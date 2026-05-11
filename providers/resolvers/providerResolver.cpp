#pragma once

#include "providers/resolvers/providerResolver.hpp"
#include "utils/utils.hpp"

oraculum::Provider resolveProviderOrThrow(const std::string& providerName) {
    const std::string normalizedProvider = toLower(providerName);
    const auto providerIt = oraculum::kProviderLookup.find(normalizedProvider);
    if (providerIt == oraculum::kProviderLookup.end()) {
        throw std::runtime_error("Unsupported provider: " + providerName);
    }
    return providerIt->second;
}
