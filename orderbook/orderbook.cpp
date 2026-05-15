#include "orderbook/orderbook.hpp"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <stdexcept>

namespace oraculum {
    std::int64_t nowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    void makeOrderBookSnapshot(
        Config& cfg,
        Connector& connector,
        Provider& provider,
        std::string& symbol,
        FileManager& fm ) 
    {   
        Connection connection = Connection::Api;

        const std::string depth = cfg.depth.value_or("5000");

        const std::filesystem::path snapshotDir =
            fm.environmentPath() / symbol / "orderbook" / "snapshots";

        std::filesystem::create_directories(snapshotDir);

        const auto endpoint =
            makeOrderBookSnapshotEndpoint(cfg, provider, connection);

        const auto response =
            connector.request(provider, connection, endpoint);
        nlohmann::json snapshot;
        try {
            snapshot = nlohmann::json::parse(response.text);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to parse Json snapshot:")+ e.what());
        }

        if (!snapshot.contains("lastUpdateId")) {
            throw std::runtime_error("Snapshot JSON does not contain lastUpdateId");
        }

        const auto lastUpdateId = snapshot["lastUpdateId"].get<std::uint64_t>();

        
        const std::string fileName =
            symbol + "-" +
            cfg.type + "-" +
            depth + "-" +
            "snapshot-" +
            std::to_string(lastUpdateId) +
            ".json";

        const std::filesystem::path snapshotPath =
            snapshotDir / fileName;

        if (std::filesystem::exists(snapshotPath)) {
            return;
        }

        nlohmann::json storedSnapshot = {
            {"provider", cfg.provider},
            {"symbol", symbol},
            {"market", "spot"},
            {"type", cfg.type},
            {"depth", depth},
            {"local_ts_ms", nowMs()},
            {"last_update_id", lastUpdateId},
            {"snapshot", snapshot}
        };

        FileHandle file = fm.createFile(snapshotPath.string());

        file.writeLine(storedSnapshot.dump(2));
    }

}
