#include "src/app/validation/validator.hpp"
#include <stdexcept>

namespace oraculum {
    Validator::Validator(Config& cfg, CacheService& cache) : cfg_(cfg), cache_(cache) {}
    void Validator::validateSymbolFromCacheOrThrow_(const std::string& symbol, SymbolToMetadata& symbols) {
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
        const auto streamType = kStreamTypeLookup.find(normalizedType);
        if (streamType == kStreamTypeLookup.end()){
            throw std::runtime_error("Please, choose a right stream type.");
        }
    }

    void Validator::validateTimeFrameOrThrow_(){
        if (cfg_.tf.has_value()){
            if (intervals.find(cfg_.tf.value()) == intervals.end()){
                throw std::runtime_error("Incorrect timeframe, please take a look at user guide for a correct one.");
            }
        }
    }

    void Validator::validateDepthOrThrow_(){
        if (cfg_.depth.has_value()){
            if (level.find(cfg_.depth.value()) == level.end()){
                throw std::runtime_error("Incorrect depth level, check user guide for supported ones.");
            }
            if (!cfg_.orderbook){
                throw std::runtime_error("Keyword 'd' is used without 'orderbook' one.");
            }
        }
    }

    void Validator::validate(){
        validateProviderOrThrow_(cfg_.provider);
        SymbolToMetadata symbols = cache_.loadOrUpdateSymbols(resolveProviderOrThrow(cfg_.provider));
        validateSymbolFromCacheOrThrow_(cfg_.symbol,symbols);
        // validateStreamTypeOrThrow_(cfg_.type);
        validateDepthOrThrow_();
        validateTimeFrameOrThrow_();
    }
}
