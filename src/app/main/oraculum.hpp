#pragma once

#include <atomic>
#include <csignal>

#include "config/config.hpp"
#include "cli/cli.hpp"
#include "filemanager/fileManager.hpp"
#include "cacheservice/CacheService.hpp"
#include "src/app/validation/validator.hpp"
#include "orderbook/constructor/orderBookConstructor.hpp"
#include "trades/aggregated/aggregateTradesStream.hpp"
#include "filemanager/registry/registry.hpp"
#include <optional>

namespace oraculum {

    std::atomic<bool> g_running{true};
    class oraculum{
    private:
        CLI cli_;
        Config cfg_;
        FileManager fm_;
        CacheService cache_;
        Validator validator_;
        std::optional<FileRegistry> registry_;

        void writeOrderBook();
        void writeAggregatedTrades();
        void setConfigMetadata();
        void handleSignal(int signal);
    public:
        oraculum(int argc, char* argv[]);
        void run();
        void terminate();
    };
}
