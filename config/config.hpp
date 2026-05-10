#pragma once
#include <string>

struct Config {
    std::string provider = "binance";
    std::string symbol;
    std::string type;
    bool writeLiveData = false;
};
