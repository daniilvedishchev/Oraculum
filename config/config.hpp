#pragma once
#include <string>
#include <optional>


namespace oraculum {
    struct Config {
        std::string provider = "binance";
        std::string symbol;
        std::string type;
        
        std::optional<std::string> tf;

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
