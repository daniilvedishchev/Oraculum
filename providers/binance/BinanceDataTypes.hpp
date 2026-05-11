#pragma once

#include <string>
#include <unordered_map>

namespace oraculum {

enum class BinanceDataType {
    Depth,
    Trades,
    HistoricalTrades,
    AggTrades,
    Klines,
    AvgPrice,
    Ticker24hr,
    TickerPrice,
    BookTicker
};

inline const std::unordered_map<std::string, BinanceDataType> kDataTypeLookup = {
    {"depth", BinanceDataType::Depth},
    {"trades", BinanceDataType::Trades},
    {"historicalTrades", BinanceDataType::HistoricalTrades},
    {"aggTrades", BinanceDataType::AggTrades},
    {"klines", BinanceDataType::Klines},
    {"avgPrice", BinanceDataType::AvgPrice},
    {"ticker24hr", BinanceDataType::Ticker24hr},
    {"tickerPrice", BinanceDataType::TickerPrice},
    {"bookTicker", BinanceDataType::BookTicker}
};

} // namespace oraculum
