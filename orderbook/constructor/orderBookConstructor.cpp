#include "orderbook/constructor/orderBookConstructor.hpp"

namespace oraculum{
    std::int64_t nowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    OrderBookConstructor::OrderBookConstructor(Config& cfg, FileManager& fm, OraculumSocket& socket) : cfg_(cfg), 
                                                DEPTH_(cfg.depth.value()), 
                                                SYMBOL_(cfg.symbol), 
                                                SOCKET_(socket), 
                                                fm_(fm) {
        
        firstUpdateReceived_= false;
        SNAPSHOT_DIR__ = fm.environmentPath() / cfg_.symbol / "orderbook" / "snapshots";
        UPDATES_DIR__ = fm.environmentPath() / cfg_.symbol / "orderbook" / "updates";

        UPDATES_PATH__ = UPDATES_DIR__/makeUpdateFileName();

        createDirectories();

        UPDATES_ = fm.createFile(UPDATES_PATH__.string());
        if (cfg_.features) {
            auto FEATURES_DIR = fm.environmentPath() / cfg.symbol / "features";
            std::filesystem::create_directories(FEATURES_DIR);
            FEATURES_ = fm.createFile(FEATURES_DIR/"features.csv");
            FEATURES_.writeLine(featureStructure);
        }
        
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

            return DepthUpdate {    
                .firstUpdateId = message.at("U").get<long long>(), 
                .lastUpdateId = message.at("u").get<long long>(),
                .bids = message.at("b").get<std::vector<std::vector<std::string>>>(),
                .asks = message.at("a").get<std::vector<std::vector<std::string>>>(),
                .raw = msg->str
            };
        }
    }

    void OrderBookConstructor::startSocket(){
        SOCKET_.socket_.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg){
            if (msg->type == ix::WebSocketMessageType::Error){
                std::cerr << "Error connecting to websocket: " << msg->errorInfo.reason << std::endl;
                return ;
            }
            if (msg->type == ix::WebSocketMessageType::Message){
                try {
                    if (auto upd = getOrderBookUpdate(msg)){
                        bool ok = SOCKET_.orderBookUpdateBuffer_.push(std::move(*upd));
                        if (!ok){
                            std::cerr<< "Buffer overflow, missed updates.";
                        }
                    }
                } catch (const std::exception& e){
                    std::cerr << e.what() << std::endl;
                }
            }
        });
        SOCKET_.socket_.start();
    }

    void OrderBookConstructor::start(){
        try {
            running_ = true;

            startSocket();

            auto snapshot = OrderBookSnapshotAfterFirstUpdate();
            localBook_.emplace(std::move(snapshot));

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

        SOCKET_.socket_.stop();
        SOCKET_.orderBookUpdateBuffer_.close();

        if (consumerThread_.joinable()) {
            consumerThread_.join();
        }
    }

    void OrderBookConstructor::consume(){
        while (running_){
            auto update = SOCKET_.orderBookUpdateBuffer_.pop();
            if (!update) break;
            else if (update.value().lastUpdateId <= localBook_->LAST_UPDATE_ID) continue;
            else if (update.value().firstUpdateId > localBook_->LAST_UPDATE_ID + 1){
                std::lock_guard<std::mutex> lock(firstUpdateMutex_);
                firstUpdateReceived_ = false;
                auto snap = OrderBookSnapshotAfterFirstUpdate();
                localBook_.emplace(std::move(snap));
                continue;
            } else {
                localBook_->applyUpdate(update.value());
                featureEngine_.emplace(*localBook_);
                const auto row = featureEngine_->compute();
                std::string featureRow = featureEngine_->toCsv(row);
                FEATURES_.writeLine(featureRow);
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
