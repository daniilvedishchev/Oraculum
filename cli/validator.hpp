#pragma once

#include <stdexcept>
#include <string>
#include <unordered_set>

#include "config/config.hpp"
#include "keywords/keywords.hpp"
#include "providers/providers.hpp"
#include "utils/utils.hpp"

inline Config parseCliArgs(int argc, char* argv[]) {
    Config cfg;

    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];
        const auto keywordIt = kKeywordLookup.find(argument);
        if (keywordIt == kKeywordLookup.end()) {
            throw std::runtime_error("Unknown argument: " + argument);
        }

        switch (keywordIt->second) {
            case Keyword::WriteLiveData:
                cfg.writeLiveData = true;
                break;
            case Keyword::Provider:
                if (i + 1 >= argc) {
                    throw std::runtime_error("Missing value after '-provider'.");
                }
                cfg.provider = argv[++i];
                break;
            case Keyword::Symbol:
                if (i + 1 >= argc) {
                    throw std::runtime_error("Missing value after '-symbol'.");
                }
                cfg.symbol = argv[++i];
                break;
            case Keyword::Type:
                if (i + 1 >= argc) {
                    throw std::runtime_error("Missing value after '-type'.");
                }
                cfg.type = argv[++i];
                break;
        }
    }

    return cfg;
}

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
