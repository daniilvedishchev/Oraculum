#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

enum class providers {
    binance
};

enum class providerEndpoint {
    api,
    websocket,
    symbols
};

struct providerUrls {
    std::string api;
    std::string websocket;
    std::string symbols;
};

inline const std::unordered_map<std::string, providers> providerLookup = {
    {"binance", providers::binance}
};

inline const std::unordered_map<providers, std::string> providerStrLookup = {
    {providers::binance, "binance"}
};

inline const std::unordered_map<std::string, providerEndpoint> endpointLookup = {
    {"api", providerEndpoint::api},
    {"websocket", providerEndpoint::websocket},
    {"symbols", providerEndpoint::symbols}
};

inline providerEndpoint parseEndpoint(const std::string& endpointName) {
    auto it = endpointLookup.find(endpointName);
    if (it == endpointLookup.end()) {
        throw std::runtime_error("Unsupported endpoint type: " + endpointName);
    }

    return it->second;
}
