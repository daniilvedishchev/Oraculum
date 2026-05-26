#include "orderbook/constructor/orderBookConstructor.hpp"

namespace oraculum{
    std::int64_t nowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    OrderBookConstructor::OrderBookConstructor(Config& cfg, FileManager& fm, OraculumSocket& socket) : cfg_(cfg), DEPTH_(cfg.depth.value()), SYMBOL_(cfg.symbol), SOCKET_(socket),fm_(fm) {
        SNAPSHOT_DIR__ = fm.environmentPath() / cfg_.symbol / "orderbook" / "snapshots";
        UPDATES_DIR__ = fm.environmentPath() / cfg_.symbol / "orderbook" / "updates";

        UPDATES_PATH__ = UPDATES_DIR__/makeUpdateFileName();

        createDirectories();

        UPDATES_ = fm.createFile(UPDATES_PATH__.string());
        
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

    void OrderBookConstructor::startSocket(){
        SOCKET_.socket_.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg){
            if (msg->type == ix::WebSocketMessageType::Error){
                std::cerr << "Error connecting to websocket: " << msg->errorInfo.reason << std::endl;
                return ;
            }
            if (msg->type == ix::WebSocketMessageType::Message){
                try {
                    const auto message = nlohmann::json::parse(msg->str);
                    if (message["e"] == "depthUpdate"){
                        auto update = DepthUpdate{.firstUpdateId = message["U"], 
                                                  .lastUpdateId = message["u"],
                                                  .raw = message};
                        
                        bool ok = SOCKET_.orderBookUpdateBuffer_.push(update);

                        if (!ok){
                            std::cerr<< "Buffer overflow, updates are lost!" << std::endl;
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

            consumerThread_ = std::thread([this](){
                consume();
            });

            const auto snapshot = getOrderBookSnapshot();

            FileHandle fileSnapshot = fm_.createFile(SNAPSHOT_PATH__.string());
            fileSnapshot.writeLine(snapshot.dump(2));
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

        if (consumerThread_.joinable()) {
            consumerThread_.join();
        }
    }

    void OrderBookConstructor::consume(){
        while (running_){
            DepthUpdate update = SOCKET_.orderBookUpdateBuffer_.pop();
            if (!SYNCRONIZED_ORDERBOOK){
                FIRST_UPDATE_ID = std::min(update.firstUpdateId,FIRST_UPDATE_ID);
            }
            UPDATES_.writeLine(update.raw.dump());
        }
    }

    nlohmann::json OrderBookConstructor::getOrderBookSnapshot()
    {   

        nlohmann::json snapshot(std::move(parseOrderBookSnapshot()));
        const auto lastUpdateId = snapshot["lastUpdateId"].get<std::uint64_t>();

        const std::string fileName = makeSnapshotFileName(lastUpdateId);

        SNAPSHOT_PATH__ = SNAPSHOT_DIR__ / fileName;

        nlohmann::json storedSnapshot = {
            {"provider", cfg_.provider},
            {"symbol", SYMBOL_},
            {"market", "spot"},
            {"type", cfg_.type},
            {"depth", DEPTH_},
            {"local_ts_ms", nowMs()},
            {"last_update_id", lastUpdateId},
            {"snapshot", snapshot}
        };

        return storedSnapshot;
    }
}