#include "src/app/validation/validator.hpp"

namespace oraculum {
    void validator::validateSymbolFromCacheOrThrow_(const std::string& symbol, const std::unordered_set<std::string>& symbols) {
        if (symbols.empty()) {
            throw std::runtime_error("Symbols cache is empty. Cannot validate symbol.");
        }

        const std::string normalizedSymbol = toUpper(symbol);
        if (symbols.find(normalizedSymbol) == symbols.end()) {
            throw std::runtime_error("Unknown symbol for selected provider: " + symbol);
        }
    }

    void validator::validateProviderOrThrow_(const std::string& provider){
        const std::string normalizedProvider = toLower(provider);
        const auto providerIt = kStringToProvider.find(normalizedProvider);
        if (providerIt == kStringToProvider.end()) {
            throw std::runtime_error("Unsupported provider: " + provider);
        }
    }

    void validator::validateStreamTypeOrThrow_(const std::string& type){
        const std::string normalizedType = toLower(type);
        auto streamType = kStreamTypeLookup.find(normalizedType);
        if (streamType == kStreamTypeLookup.end()){
            throw std::runtime_error("Please, choose a right stream type.");
        }
    }
}
