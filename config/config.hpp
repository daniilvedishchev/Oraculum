#pragma once
#include <string>
#include <optional>

#include "infrastructure/market/intervals.hpp"

namespace oraculum {
    struct Config {
        std::string provider = "binance";
        std::string symbol;
        std::string type;
        
        std::optional<std::string> tf;
        std::optional<std::string> depth;
        std::optional<std::string> speed;

        bool writeLiveData = false;
    };
}
