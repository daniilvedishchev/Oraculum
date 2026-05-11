#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace oraculum {

enum class Provider {
    Binance
};

enum class ProviderEndpoint {
    Api,
    WebSocket,
    Symbols
};

inline const std::unordered_map<std::string, Provider> kProviderLookup = {
    {"binance", Provider::Binance}
};

inline const std::unordered_map<Provider, std::string> kProviderNameByProvider = {
    {Provider::Binance, "binance"}
};

inline const std::unordered_map<std::string, ProviderEndpoint> kEndpointLookup = {
    {"api", ProviderEndpoint::Api},
    {"websocket", ProviderEndpoint::WebSocket},
    {"symbols", ProviderEndpoint::Symbols}
};

inline ProviderEndpoint parseProviderEndpoint(const std::string& endpointName) {
    auto it = kEndpointLookup.find(endpointName);
    if (it == kEndpointLookup.end()) {
        throw std::runtime_error("Unsupported endpoint type: " + endpointName);
    }

    return it->second;
}

} // namespace oraculum
