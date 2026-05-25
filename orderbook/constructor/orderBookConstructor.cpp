#include "orderbook/constructor/orderBookConstructor.hpp"

namespace oraculum{

    std::int64_t nowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    OrderBookConstructor::OrderBookConstructor(Config& cfg, FileManager& fm, OraculumSocket& socket) : cfg_(cfg), DEPTH_(cfg.depth.value()), SYMBOL_(cfg.symbol), SOCKET_(socket) {
        SNAPSHOT_DIR__ = fm.environmentPath() / cfg_.symbol / "orderbook" / "snapshots";
        UPDATES_DIR__ = fm.environmentPath() / cfg_.symbol / "orderbook" / "updates";

        createDirectories();
        const auto snapshot(std::move(getOrderBookSnapshot()));
        
        FileHandle file = fm.createFile(SNAPSHOT_PATH__.string());
        file.writeLine(snapshot.dump(2));
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

    void OrderBookConstructor::startSocket(){
        SOCKET_.socket_.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg){
            if (msg->type == ix::WebSocketMessageType::Error){
                throw std::runtime_error("Error connecting to websocket: " + msg->errorInfo.reason + "\n");
            }
            if (msg->type == ix::WebSocketMessageType::Message){
                try {
                    const auto message = nlohmann::json::parse(msg->str);
                    if (message["E"] == "DepthUpdate"){
                        auto update = DepthUpdate{.firstUpdateId = message["U"], .lastUpdateId = message["u"], .raw = message};
                        SOCKET_.orderBookUpdateBuffer_.push(update);
                    }
                } catch (const std::exception& e){
                    std::cerr << e.what() << std::endl;
                }

            }
        });

    }

    nlohmann::json OrderBookConstructor::getOrderBookSnapshot()
    {   

        nlohmann::json snapshot(std::move(parseOrderBookSnapshot()));
        const auto lastUpdateId = snapshot["lastUpdateId"].get<std::uint64_t>();

        const std::string fileName = makeSnapshotFileName(lastUpdateId);

        SNAPSHOT_PATH__ = SNAPSHOT_DIR__ / fileName;

        if (std::filesystem::exists(SNAPSHOT_PATH__)) {
            throw std::runtime_error("Already exist.");
        }

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