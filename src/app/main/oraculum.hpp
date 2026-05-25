#pragma once

#include "config/config.hpp"
#include "cli/cli.hpp"
#include "filemanager/fileManager.hpp"
#include "cacheservice/CacheService.hpp"
#include "src/app/validation/validator.hpp"
#include "orderbook/constructor/orderBookConstructor.hpp"

namespace oraculum {
    class oraculum{
    private:
        CLI cli_;
        Config cfg_;
        FileManager fm_;
        CacheService cache_;
        Validator validator_;

        void writeOrderBook();
    public:
        oraculum(int argc, char* argv[]);
        void run();
    };
}
