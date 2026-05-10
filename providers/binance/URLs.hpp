#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "providers/providers.hpp"
#include "providers/binance/binance.hpp"

namespace oraculum {

inline const std::unordered_map<Provider, ProviderUrls> kProviderUrlsByProvider = {
    {Provider::Binance, kBinanceUrls}
};

inline const std::string& resolveProviderUrl(Provider provider, ProviderEndpoint endpoint) {
    auto providerIt = kProviderUrlsByProvider.find(provider);
    if (providerIt == kProviderUrlsByProvider.end()) {
        throw std::runtime_error("This provider is not supported.");
    }

    const ProviderUrls& urls = providerIt->second;
    switch (endpoint) {
        case ProviderEndpoint::Api:
            return urls.api;
        case ProviderEndpoint::WebSocket:
            return urls.websocket;
        case ProviderEndpoint::Symbols:
            return urls.symbols;
    }

    throw std::runtime_error("Unsupported endpoint selector.");
}

} // namespace oraculum
