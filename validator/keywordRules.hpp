#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include "namespace/namespace.hpp"
#include "keywords/keywords.hpp"
#include "providers/providers.hpp"

struct rule {
    bool mandatory;
    Oraculum::value valuebydefault;
    std::function<bool(const char* args[],const int argc,size_t idx)> validate;
};

inline std::unordered_map<keywords,rule> keywordtorule = {
    {keywords::_PROVIDER, rule{
        false,
        providers::binance,
        [](const char* args[],const int argc,size_t idx) -> bool {
            if (idx+1 >= argc || std::isdigit(static_cast<unsigned char>(args[idx + 1][0]))){
                throw std::runtime_error("Provider name not specified, delete '-provider' keyword to use value by default.");
                return false;
            }
            return providerlookup.find(static_cast<std::string>(args[idx+1])) != providerlookup.end();
        }
    }}
};

// inline bool validateKeywordValue(const keywords keyword, const Oraculum::value& value) {
//     const auto it = keywordtorule.find(keyword);
//     if (it == keywordtorule.end()) {
//         return false;
//     }

//     return it->second.validate(value);
// }
