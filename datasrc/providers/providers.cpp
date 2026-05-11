#include "datasrc/providers/providers.hpp"
#include "datasrc/binance/urls/binanceUrls.hpp"

namespace oraculum {
    const std::unordered_map<Provider,ProviderBase> kProviderToUrls = {
        {Provider::Binance, kBinanceUrls}
    };
    const std::unordered_map<std::string,Provider> kStringToProvider = {
        {"binance", Provider::Binance}
    };
}