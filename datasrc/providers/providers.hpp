#pragma once 

#include <unordered_map>
#include "datasrc/providers/baseUrl.hpp"

namespace oraculum {
    enum class Provider {
        Binance
    };
    extern const std::unordered_map<Provider,ProviderBase> kProviderToUrls;
    extern const std::unordered_map<std::string,Provider> kStringToProvider;
    extern const std::unordered_map<Provider,std::string> kProviderToString;
}