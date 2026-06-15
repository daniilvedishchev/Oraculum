#pragma once

#include "datasrc/providers/baseUrl.hpp"

namespace oraculum {
    inline const ProviderBase kBinanceBaseUrls = {
        "https://api.binance.com/api/v3/",
        "wss://stream.binance.com:443/ws/",
        "wss://fstream.binance.com/market/ws/",
        "https://api.binance.com/api/v3/exchangeInfo"
    };
}
