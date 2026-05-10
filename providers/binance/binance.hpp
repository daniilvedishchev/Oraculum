#pragma once

#include "providers/providers.hpp"

namespace oraculum {

inline const ProviderUrls kBinanceUrls = {
    "https://fapi.binance.com/fapi/v1/",
    "wss://fstream.binance.com/public/ws/",
    "https://api.binance.com/api/v3/exchangeInfo"
};

} // namespace oraculum
