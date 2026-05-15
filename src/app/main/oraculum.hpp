#pragma once

#include "config/config.hpp"
#include "cli/cli.hpp"
#include "filemanager/fileManager.hpp"
#include "cacheservice/CacheService.hpp"
#include "src/app/validation/validator.hpp"
#include "orderbook/orderbook.hpp"
#include "connector/connector.hpp"
namespace oraculum {
    class oraculum{
    private:
        CLI cli_;
        Config cfg_;
        FileManager fm_;
        Connector connector_;
        CacheService cache_;
        Validator validator_;
    public:
        oraculum(int argc, char* argv[]);
        void run();
    };
}
