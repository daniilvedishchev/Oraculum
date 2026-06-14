#pragma once 

#include <string> 
#include <filesystem>
#include <thread>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "config/config.hpp"
#include "connector/builder/builder.hpp"
#include "filemanager/registry/registry.hpp"
#include "orderbook/depth/depthUpdate.hpp"
#include "socket/ring/ringBuffer.hpp"
#include "connector/retry/retry.hpp"
#include "datasrc/endpoints/endpoints.hpp"
#include "filemanager/fileManager.hpp"
#include "socket/oraculumSocket/oraculumSocket.hpp"
#include "orderbook/localbook/LocalOrderBook.hpp"
#include "orderbook/features/structure/header.hpp"
#include "orderbook/features/FeatureEngine.hpp"
#include "cacheservice/CacheService.hpp"

namespace oraculum {
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
        std::optional<LocalOrderBook> localBook_;
        ~OrderBookConstructor();
        nlohmann::json OrderBookSnapshotAfterFirstUpdate();
        void start();
        void stop();
    };

}
