#pragma once

#include <stdexcept>
#include <string>
#include <unordered_set>

#include "config/config.hpp"
#include "keywords/keywords.hpp"
#include "providers/providers.hpp"
#include "utils/utils.hpp"

inline std::unordered_set<Keyword> requiredKeywords = {Keyword::Symbol,Keyword::Type,Keyword::Provider};

inline oraculum::Provider resolveProvider(const std::string& providerName) {
    const std::string normalizedProvider = toLower(providerName);
    const auto providerIt = oraculum::kProviderLookup.find(normalizedProvider);
    if (providerIt == oraculum::kProviderLookup.end()) {
        throw std::runtime_error("Unsupported provider: " + providerName);
    }
    return providerIt->second;
}

inline void validateSymbolWithCache(const std::string& symbol, const std::unordered_set<std::string>& symbols) {
    if (symbols.empty()) {
        throw std::runtime_error("Symbols cache is empty. Cannot validate symbol.");
    }

    const std::string normalizedSymbol = toUpper(symbol);
    if (symbols.find(normalizedSymbol) == symbols.end()) {
        throw std::runtime_error("Unknown symbol for selected provider: " + symbol);
    }
}
