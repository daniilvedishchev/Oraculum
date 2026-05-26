#pragma once

#include "datasrc/providers/providers.hpp"

namespace oraculum {
    inline const ProviderBase kBinanceBaseUrls = {
        "https://fapi.binance.com/fapi/v1/",
        "wss://stream.binance.com:443/ws/",
        "https://api.binance.com/api/v3/exchangeInfo"
    };
}