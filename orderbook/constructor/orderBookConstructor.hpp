#pragma once 

#include <string> 
#include <filesystem>
#include <nlohmann/json.hpp>
#include "config/config.hpp"
#include "connector/builder/builder.hpp"
#include "connector/retry/retry.hpp"
#include "datasrc/endpoints/endpoints.hpp"
#include "filemanager/fileManager.hpp"
namespace oraculum {
    class OrderBookConstructor {
    private:
        Config& cfg_;
        std::string DEPTH_;
        std::string SYMBOL_;
        std::string LAST_SNAPSHOT_ID;

        std::filesystem::path SNAPSHOT_DIR__;
        std::filesystem::path SNAPSHOT_PATH__;
        std::filesystem::path UPDATES_DIR__;


        std::string makeSnapshotFileName(const std::uint64_t& lastUpdateId);

        nlohmann::json parseOrderBookSnapshot();

        void createDirectories();

        void markOrderBookUpdates_();

        void makeFileName(bool snapshot = false);



    public:
        OrderBookConstructor(Config& cfg, FileManager& fm);
        nlohmann::json getOrderBookSnapshot();
    };

}
