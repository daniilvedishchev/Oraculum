#pragma once
#include <unordered_map>
#include <string>
namespace oraculum {
    enum class Keyword {
        OrderBook,
        Provider,
        Symbol,
        Depth,
        Speed,
        TimeFrame,
        AggregatedTrades,
        Liquidations,
        Features
    };

    inline const std::unordered_map<std::string, Keyword> kKeywordLookup = {
        {"-orderbook", Keyword::OrderBook},
        {"-provider", Keyword::Provider},
        {"-symbol", Keyword::Symbol},
        {"-d", Keyword::Depth},
        {"-s", Keyword::Speed},
        {"-tf", Keyword::TimeFrame},
        {"-trades", Keyword::AggregatedTrades},
        {"-liquidations", Keyword::Liquidations},
        {"-features", Keyword::Features}
    };
}
