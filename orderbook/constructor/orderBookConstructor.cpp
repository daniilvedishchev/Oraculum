#include "orderbook/constructor/orderBookConstructor.hpp"
#include "datasrc/endpoints/endpoints.hpp"
#include "orderbook/depth/depthUpdate.hpp"
#include <functional>

namespace oraculum{
    std::int64_t nowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    OrderBookConstructor::OrderBookConstructor(Config& cfg, FileManager& fm, CacheService& cache) : cfg_(cfg), 
            DEPTH_(cfg.depth.value()), 
            SYMBOL_(cfg.symbol), 
            fm_(fm),
            cache_(cache)
        {
        
        firstUpdateReceived_= false;
        FEATURES_ON = cfg_.features;
        SNAPSHOT_DIR__ = fm.environmentPath() / cfg_.symbol / "orderbook" / "snapshots";
        UPDATES_DIR__ = fm.environmentPath() / cfg_.symbol / "orderbook" / "updates";

        UPDATES_PATH__ = UPDATES_DIR__/makeUpdateFileName();

        createDirectories();

        UPDATES_ = fm.createFile(UPDATES_PATH__.string());
        if (FEATURES_ON) {
            auto FEATURES_DIR = fm.environmentPath() / cfg.symbol / "features";
            std::filesystem::create_directories(FEATURES_DIR);
            FEATURES_ = fm.createFile((FEATURES_DIR / "features.csv").string());
            FEATURES_.writeLine(featureStructure);
        }

        orderBookUpdateMsgCallback = [this](const ix::WebSocketMessagePtr& msg) {
            try {
                if (auto upd = getOrderBookUpdate(msg)) {
                    bool ok = socket_.value().buffer_.push(std::move(*upd));
                    if (!ok) {
                        std::cerr << "Buffer overflow, missed updates.";
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        };

        socket_.emplace(cfg_.provider,
                        buildUrl(resolveProviderOrThrow(cfg_.provider),
                        Connection::WebSocket,
                        makeOrderBookUpdateEndpoint(cfg_)),
                        orderBookUpdateMsgCallback);
        
    }

    nlohmann::json OrderBookConstructor::parseOrderBookSnapshot(){
        std::string endpoint = makeOrderBookSnapshotEndpoint(cfg_);

        std::string url = buildUrl(resolveProviderOrThrow(cfg_.provider),Connection::Api,endpoint); 
        const auto response = requestRetryOrThrow(url);

        nlohmann::json snapshot;
        
        try {
            snapshot = nlohmann::json::parse(response.text);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to parse Json snapshot:")+ e.what());
        }

        if (!snapshot.contains("lastUpdateId")) {
            throw std::runtime_error("Snapshot JSON does not contain lastUpdateId");
        }

        return snapshot;
    }

    void OrderBookConstructor::createDirectories(){
        std::filesystem::create_directories(SNAPSHOT_DIR__);
        std::filesystem::create_directories(UPDATES_DIR__);
    }
    
    std::string OrderBookConstructor::makeSnapshotFileName(const std::uint64_t& lastUpdateId){
       return SYMBOL_ + "-" +
            "DEPTH" + "-" +
            DEPTH_ + "-" +
            "snapshot-" +
            std::to_string(lastUpdateId) +
            ".json";
    }

    std::string OrderBookConstructor::makeUpdateFileName(){
       return SYMBOL_ + "-" +
            "DEPTH" + "-" +
            DEPTH_ + "-" +
            "update" +
            ".json";
    }

    /** 
        * @brief This function sets up the first updateID , usefull for orderbook reconstruction
        * and moves update to the buffer
    */
    std::optional<DepthUpdate> OrderBookConstructor::getOrderBookUpdate(const ix::WebSocketMessagePtr& msg){
        {
            std::lock_guard<std::mutex> lock(firstUpdateMutex_);
            const auto message = nlohmann::json::parse(msg->str);

            if (!firstUpdateReceived_){
                firstUpdateReceived_ = true;
                firstUpdateCv_.notify_all();
            }

            DepthUpdate update;
            update.raw = msg->str;
            update.firstUpdateId = message.at("U").get<long long>();
            update.lastUpdateId = message.at("u").get<long long>();
            update.lastUpdateTs = message.at("E").get<long long>();
            update.bids = message.at("b").get<std::vector<std::vector<std::string>>>();
            update.asks = message.at("a").get<std::vector<std::vector<std::string>>>();
            return update;
        }
    }

    void OrderBookConstructor::startSocket(){
        socket_.value().socket_.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg){
            if (msg->type == ix::WebSocketMessageType::Error){
                std::cerr << "Error connecting to websocket: " << msg->errorInfo.reason << std::endl;
                return ;
            }
            if (msg->type == ix::WebSocketMessageType::Message){
                
            }
        });
        socket_.value().socket_.start();
    }

    void OrderBookConstructor::start(){
        try {
            running_ = true;

            startSocket();

            auto snapshot = OrderBookSnapshotAfterFirstUpdate();
            MetaData meta = cache_.getMetaBySymbolOrThrow(cfg_.symbol); 
            localBook_.emplace(std::move(snapshot),meta.tickSize,meta.stepSize);

            if (FEATURES_ON) featureEngine_.emplace(*localBook_,FEATURES_);

            consumerThread_ = std::thread([this]() {
                consume();
            });

        } catch (std::exception& e){
            std::cerr << "[start error] " << e.what() << std::endl;
            stop();
        }
    }

    OrderBookConstructor::~OrderBookConstructor() {
        std::cerr << "[DEBUG] destructor called\n";
        stop();
    }

    void OrderBookConstructor::stop(){
        std::cerr << "[DEBUG] stop called\n";
        running_= false;

        socket_.value().socket_.stop(1000, "Normal closure");
        socket_.value().buffer_.close();

        if (consumerThread_.joinable()) {
            consumerThread_.join();
        }
    }

    void OrderBookConstructor::consume(){
        while (running_){
            auto update = socket_.value().buffer_.pop();
            if (!update) break;

            bool needResync = update.value().firstUpdateId > localBook_->LAST_UPDATE_ID + 1;
            bool oldUpdate = update.value().lastUpdateId <= localBook_->LAST_UPDATE_ID;
            
            if (oldUpdate) continue;
            else if (needResync){
                {
                    std::lock_guard<std::mutex> lock(firstUpdateMutex_);
                    firstUpdateReceived_ = false;
                }
                auto snap = OrderBookSnapshotAfterFirstUpdate(); 
                localBook_.emplace(std::move(snap),cfg_.tickSize,cfg_.stepSize);

                if (FEATURES_ON) featureEngine_.emplace(*localBook_,FEATURES_);

                continue;
            } else {
                localBook_->applyUpdate(update.value());
                if (FEATURES_ON) featureEngine_.value().features_1s(update.value());
                UPDATES_.writeLine(update->raw);
            }
        }
    }
    
    nlohmann::json OrderBookConstructor::OrderBookSnapshotAfterFirstUpdate() {   
        
        {   
            std::unique_lock<std::mutex> lock(firstUpdateMutex_);
            firstUpdateCv_.wait(lock,[this]{
                return firstUpdateReceived_ || !running_;
            });
        }

        snapshot = parseOrderBookSnapshot();
        SNAPSHOT_ID = snapshot.at("lastUpdateId").get<long long>();

        const std::string fileName = makeSnapshotFileName(SNAPSHOT_ID);

        SNAPSHOT_PATH__ = SNAPSHOT_DIR__ / fileName;

        nlohmann::json storedSnapshot = {
            {"provider", cfg_.provider},
            {"symbol", SYMBOL_},
            {"market", "spot"},
            {"type", cfg_.type},
            {"depth", DEPTH_},
            {"local_ts_ms", nowMs()},
            {"last_update_id", SNAPSHOT_ID},
            {"snapshot", snapshot}
        };

        FileHandle fileSnapshot = fm_.createFile(SNAPSHOT_PATH__.string());
        fileSnapshot.writeLine(storedSnapshot.dump(2));

        return storedSnapshot;
    }
}
