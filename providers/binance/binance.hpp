#pragma once
#include <unordered_map>
#include <string>

inline std::unordered_map<std::string,std::string> _binance = {
    {"binancesocket","wss://fstream.binance.com/public/ws/"},
    {"binanceapi","https://fapi.binance.com/fapi/v1/"},
    {"binancesymbols","https://api.binance.com/api/v3/exchangeInfo"}
};