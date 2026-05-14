#pragma once

#include <string>
#include <unordered_set>


#include "utils/utils.hpp"
#include "datasrc/providers/providers.hpp"
#include "datasrc/binance/streamTypes/allStreamTypes.hpp"
#include "datasrc/resolvers/providerResolver.hpp"
#include "src/app/validation/requiredFields.hpp"
#include "datasrc/binance/streamTypes/allStreamTypes.hpp"
#include "infrastructure/market/intervals.hpp"

class Config;
class CacheService;

namespace oraculum {
    class Validator {
        private:
            Config& cfg_;
            CacheService& cache_;
            
            void validateSymbolFromCacheOrThrow_(const std::string& symbol, const std::unordered_set<std::string>& symbols);
            void validateProviderOrThrow_(const std::string& symbol);
            void validateStreamTypeOrThrow_(const std::string& type);
            void validateDepthOrThrow_();
            void validateSpeedOrThrow_();
            void validateTimeFrameOrThrow_();
        public:
            Validator(Config& cfg,CacheService& cache);
            void validate();
        };
}

