#pragma once 

#include <string> 
#include <filesystem>
#include <thread>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

#include "config/config.hpp"
#include "connector/builder/builder.hpp"
#include "socket/ring/ringBuffer.hpp"
#include "connector/retry/retry.hpp"
#include "datasrc/endpoints/endpoints.hpp"
#include "filemanager/fileManager.hpp"
#include "socket/oraculumSocket/oraculumSocket.hpp"

#define NULL 0

namespace oraculum {
    class OrderBookConstructor {
    private:
        Config& cfg_;
        OraculumSocket& SOCKET_;
        FileManager& fm_;

        std::string DEPTH_;
        std::string SYMBOL_;
        std::string LAST_SNAPSHOT_ID;

        std::filesystem::path SNAPSHOT_DIR__;
        std::filesystem::path SNAPSHOT_PATH__;
        std::filesystem::path UPDATES_DIR__;
        std::filesystem::path UPDATES_PATH__;

        long long FIRST_UPDATE_ID;

        FileHandle UPDATES_;
        FileHandle SNAPSHOT_;

        std::atomic<bool> running_{false};
        std::thread consumerThread_;

        std::atomic<bool> SYNCRONIZED_ORDERBOOK{false};

        std::string makeSnapshotFileName(const std::uint64_t& lastUpdateId);
        std::string makeUpdateFileName();

        nlohmann::json parseOrderBookSnapshot();

        void startSocket();
        void consume();

        void createDirectories();
    public:
        OrderBookConstructor(Config& cfg, FileManager& fm, OraculumSocket& socket);
        ~OrderBookConstructor();
        nlohmann::json getOrderBookSnapshot();
        void start();
        void stop();
    };

}
