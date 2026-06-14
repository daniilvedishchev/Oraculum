#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

#include <ixwebsocket/IXWebSocketMessage.h>
#include <nlohmann/json.hpp>

#include "config/config.hpp"
#include "orderbook/depth/depthUpdate.hpp"
#include "orderbook/features/FeatureEngine.hpp"
#include "orderbook/localbook/LocalOrderBook.hpp"
#include "socket/oraculumSocket/oraculumSocket.hpp"

namespace oraculum {
    class CacheService;
    class FileManager;
    class FileRegistry;

    class OrderBookConstructor {
    private:
        Config& cfg_;
        FileManager& fm_;
        CacheService& cache_;
        FileRegistry& registry_;

        std::optional<FeatureEngine> featureEngine_;
        std::optional<OraculumSocket<DepthUpdate>> socket_;

        long long SNAPSHOT_ID;

        std::atomic<bool> firstUpdateReceived_;
        std::condition_variable firstUpdateCv_;
        std::mutex firstUpdateMutex_;

        nlohmann::json snapshot;

        std::atomic<bool> running_{false};
        std::thread consumerThread_;

        nlohmann::json parseOrderBookSnapshot();
        std::optional<DepthUpdate> getOrderBookUpdate(const ix::WebSocketMessagePtr& msg);

        void startSocket();
        void consume();

        std::function<void(const ix::WebSocketMessagePtr&)> orderBookUpdateMsgCallback;

    public:
        OrderBookConstructor(Config& cfg, FileManager& fm, CacheService& cache, FileRegistry& registry);
        ~OrderBookConstructor();

        std::optional<LocalOrderBook> localBook_;

        nlohmann::json OrderBookSnapshotAfterFirstUpdate();
        void start();
        void stop();
    };
}
