#pragma once 

#include <string> 
#include <filesystem>
#include <nlohmann/json.hpp>
#include "config/config.hpp"
namespace oraculum {
    class OrderBookConstructor {
    private:
        std::string& DEPTH_;
        std::string& SYMBOL_;
        std::string LAST_SNAPSHOT_ID;

        std::filesystem::path SNAPSHOT_DIR__;
        std::filesystem::path UPDATES_DIR__;


        nlohmann::json getOrderBookSnapshot();
        void markOrderBookUpdates_();

        void makeFileName(bool snapshot = false);


    public:
        OrderBookConstructor(Config& cfg);
        nlohmann::json getOrderBookSnapshot();
    };

}
