#pragma once

#include <optional>

#include <nlohmann/json.hpp>
#include <thread>
#include <sstream>
#include <cmath>
#include <atomic>

#include "filemanager/file/file.hpp"
#include "socket/oraculumSocket/oraculumSocket.hpp"
#include "config/config.hpp"
#include "filemanager/fileManager.hpp"
#include "trades/structure/trades.h"
#include "connector/builder/builder.hpp"


#include "status/status.hpp"

namespace oraculum {
    class AggregateTradeStream {
        private:
            std::atomic<Status> status_;
            std::optional<OraculumSocket<AggregateTrade>> socket_;
            Config& cfg_;
            FileHandle& file_;
            std::thread thread_;
            socketCallback aggregateTradesMsgCallback;
            AggregateTrade makeTrade(const ix::WebSocketMessagePtr& msg);
            std::string tradeToCsv(AggregateTrade& trade);

            void consumeTrades();
        public:
            AggregateTradeStream(Config& config, FileHandle& file);
            void runAggregateTradeStream();
            void stopAggregateTradeStream();
    };
}
