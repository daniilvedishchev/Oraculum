#include "src/app/validation/validator.hpp"

namespace oraculum {
    Validator::Validator(Config& cfg,CacheService& cache) : cfg_(cfg), cache_(cache){}
    void Validator::validateSymbolFromCacheOrThrow_(const std::string& symbol, const std::unordered_set<std::string>& symbols) {
        if (symbols.empty()) {
            throw std::runtime_error("Symbols cache is empty. Cannot validate symbol.");
        }

        const std::string normalizedSymbol = toUpper(symbol);
        if (symbols.find(normalizedSymbol) == symbols.end()) {
            throw std::runtime_error("Unknown symbol for selected provider: " + symbol);
        }
    }

    void Validator::validateProviderOrThrow_(const std::string& provider){
        const std::string normalizedProvider = toLower(provider);
        const auto providerIt = kStringToProvider.find(normalizedProvider);
        if (providerIt == kStringToProvider.end()) {
            throw std::runtime_error("Unsupported provider: " + provider);
        }
    }

    void Validator::validateStreamTypeOrThrow_(const std::string& type){
        const std::string normalizedType = toLower(type);
        auto streamType = kStreamTypeLookup.find(normalizedType);
        if (streamType == kStreamTypeLookup.end()){
            throw std::runtime_error("Please, choose a right stream type.");
        }
    }

    void Validator::validate(){
        validateProviderOrThrow_(cfg_.provider);
        validateStreamTypeOrThrow_(cfg_.type);
        std::unordered_set<std::string> symbols = cache_.loadOrUpdateSymbols(resolveProviderOrThrow(cfg_.provider));
        validateSymbolFromCacheOrThrow_(cfg_.symbol,symbols);
    }
}
