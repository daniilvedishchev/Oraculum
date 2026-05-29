#pragma once

#include "datasrc/providers/providers.hpp"

namespace oraculum {
    inline const ProviderBase kBinanceBaseUrls = {
        "https://api.binance.com/api/v3/",
        "wss://stream.binance.com:443/ws/",
        "https://api.binance.com/api/v3/exchangeInfo"
    };
}