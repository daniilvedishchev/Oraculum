#pragma once 

#include <string> 
#include <filesystem>
#include <thread>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <mutex>
#include <condition_variable>

#include "config/config.hpp"
#include "connector/builder/builder.hpp"
#include "socket/ring/ringBuffer.hpp"
#include "connector/retry/retry.hpp"
#include "datasrc/endpoints/endpoints.hpp"
#include "filemanager/fileManager.hpp"
#include "socket/oraculumSocket/oraculumSocket.hpp"
#include "orderbook/localbook/LocalOrderBook.hpp"

namespace oraculum {
    class OrderBookConstructor {
    private:
        Config& cfg_;
        OraculumSocket& SOCKET_;
        FileManager& fm_;

        std::string DEPTH_;
        std::string SYMBOL_;
        long long SNAPSHOT_ID;

        std::atomic<bool> firstUpdateReceived_;
        std::condition_variable firstUpdateCv_;
        std::mutex firstUpdateMutex_;

        std::filesystem::path SNAPSHOT_DIR__;
        std::filesystem::path SNAPSHOT_PATH__;
        std::filesystem::path UPDATES_DIR__;
        std::filesystem::path UPDATES_PATH__;

        nlohmann::json snapshot;

        FileHandle UPDATES_;
        FileHandle SNAPSHOT_;

        std::atomic<bool> running_{false};
        std::thread consumerThread_;

        std::string makeSnapshotFileName(const std::uint64_t& lastUpdateId);
        std::string makeUpdateFileName();

        nlohmann::json parseOrderBookSnapshot();
        std::optional<DepthUpdate> getOrderBookUpdate(const ix::WebSocketMessagePtr& msg);

        void startSocket();
        void consume();

        void createDirectories();
    public:
        OrderBookConstructor(Config& cfg, FileManager& fm, OraculumSocket& socket);
        std::optional<LocalOrderBook> localBook_;
        ~OrderBookConstructor();
        nlohmann::json OrderBookSnapshotAfterFirstUpdate();
        void start();
        void stop();
    };

}
