#pragma once

#include <string>
#include <unordered_set>

#include "config/config.hpp"
#include "utils/utils.hpp"
#include "datasrc/providers/providers.hpp"
#include "datasrc/binance/streamTypes/allStreamTypes.hpp"
#include "src/app/validation/requiredFields.hpp"

namespace oraculum {
    class validator {
        private:
            Config& cfg_;
            void validateSymbolFromCacheOrThrow_(const std::string& symbol, const std::unordered_set<std::string>& symbols);
            void validateProviderOrThrow_(const std::string& symbol);
            void validateStreamTypeOrThrow_(const std::string& type);
        public:
            validator(Config& cfg);
        };
}

