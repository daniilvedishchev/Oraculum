#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "providers/providers.hpp"
#include "providers/binance/binance.hpp"

inline const std::unordered_map<providers, providerUrls> providerUrlsByProvider = {
    {providers::binance, binanceUrls}
};

inline const std::string& resolveProviderUrl(providers provider, providerEndpoint endpoint) {
    auto providerIt = providerUrlsByProvider.find(provider);
    if (providerIt == providerUrlsByProvider.end()) {
        throw std::runtime_error("This provider is not supported.");
    }

    const providerUrls& urls = providerIt->second;
    switch (endpoint) {
        case providerEndpoint::api:
            return urls.api;
        case providerEndpoint::websocket:
            return urls.websocket;
        case providerEndpoint::symbols:
            return urls.symbols;
    }

    throw std::runtime_error("Unsupported endpoint selector.");
}
