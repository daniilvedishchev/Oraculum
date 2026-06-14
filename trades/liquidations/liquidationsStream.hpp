#pragma once

#include <optional>

#include <nlohmann/json.hpp>
#include <thread>
#include <sstream>
#include <cmath>
#include <atomic>

#include "filemanager/file/file.hpp"
#include "socket/oraculumSocket/oraculumSocket.hpp"
#include "filemanager/registry/registry.hpp"
#include "config/config.hpp"
#include "filemanager/fileManager.hpp"
#include "trades/liquidations/liquidations.hpp"
#include "connector/builder/builder.hpp"


#include "status/status.hpp"

namespace oraculum {
    class LiquidationsStream {
        private:
            std::atomic<Status> status_;
            std::optional<OraculumSocket<Liquidation>> socket_;
            Config& cfg_;
            FileRegistry& registry_;
            std::thread thread_;
            socketCallback liquidationsMsgCallback;
            Liquidation makeLiquidation(const ix::WebSocketMessagePtr& msg);
            std::string liquidationToCsv(Liquidation& liquidation);

            void consumeLiquidations();
        public:
            LiquidationsStream(Config& config, FileRegistry& registry);
            void start();
            void stop();
    };
}
