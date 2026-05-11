#pragma once

#include <string>
#include <unordered_set>

#include "config/config.hpp"

class validator {
private:
    Config& cfg_;
    void validateSymbolFromCacheOrThrow_(const std::string& symbol, const std::unordered_set<std::string>& symbols);
    void validateProviderOrThrow_(const std::string& symbol, const std::unordered_set<std::string>& symbols);
    void validateTypeOrThrow_();
public:
    validator(Config& cfg);
};

