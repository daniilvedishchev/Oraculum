#pragma once

#include <string>
#include <unordered_set>

#include "utils/utils.hpp"
#include "datasrc/providers/providers.hpp"
#include "datasrc/binance/streamTypes/allStreamTypes.hpp"
#include "datasrc/resolvers/providerResolver.hpp"
#include "infrastructure/market/intervals.hpp"
#include "datasrc/binance/streamTypes/depth/levels.hpp"
#include "config/config.hpp"
#include "cacheservice/CacheService.hpp"

namespace oraculum {
    class Validator {
        private:
            Config& cfg_;
            CacheService& cache_;
            
            void validateSymbolFromCacheOrThrow_(const std::string& symbol, METADATA& symbols);
            void validateProviderOrThrow_(const std::string& symbol);
            void validateStreamTypeOrThrow_(const std::string& type);
            void validateDepthOrThrow_();
            void validateSpeedOrThrow_();
            void validateTimeFrameOrThrow_();
        public:
            Validator(Config& cfg, CacheService& cache);
            void validate();
        };
}
