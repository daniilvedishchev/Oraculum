#pragma once
#include <string>
#include <optional>


namespace oraculum {
    struct Config {
        std::string provider = "binance";
        std::string symbol;
        std::string type;

        double stepSize;
        double tickSize;
        
        std::optional<std::string> depth;
        std::optional<std::string> speed;

        bool writeLiveData = false;
        bool snapshots = false;
        bool updates = false;
        bool features = true;
        bool aggTrades = false;
        bool liquidations = false;
    };
}
