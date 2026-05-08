#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include "namespace.hpp"
#include "keywords/keywords.hpp"
#include "providers/providers.hpp"

struct rule {
    bool mandatory;
    Oraculum::value valuebydefault;
    std::function<bool()> validate;
};

inline std::unordered_map<keywords,rule> keywordtorule = {
    {keywords::_PROVIDER,rule{.mandatory = true,.valuebydefault = providers::binance}}
};