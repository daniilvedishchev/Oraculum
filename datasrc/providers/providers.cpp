#include "datasrc/providers/providers.hpp"
#include "datasrc/binance/urls/binanceBaseUrls.hpp"

namespace oraculum {
    const std::unordered_map<Provider,ProviderBase> kProviderToUrls = {
        {Provider::Binance, kBinanceBaseUrls}
    };
    const std::unordered_map<std::string,Provider> kStringToProvider = {
        {"binance", Provider::Binance}
    };
    const std::unordered_map<Provider,std::string> kProviderToString = {
        {Provider::Binance,"binance"}
    };
}
