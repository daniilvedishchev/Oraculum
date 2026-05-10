#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include "config/config.hpp"
#include "namespace/namespace.hpp"
#include "keywords/keywords.hpp"
#include "providers/providers.hpp"

struct rule {
    Oraculum::value mandatory;
    Oraculum::value valuebydefault;
    std::function<bool(config& cfg, const char* args[], const int argc, size_t idx)> validate;
};

inline std::unordered_map<keywords,rule> keywordtorule = {
    {keywords::_WRITELIVEDATA, rule{
        true,
        "None",
        [](config& cfg,const char* args[],const int argc,size_t idx) -> bool {
            if (argc > 2){
                throw std::runtime_error(" '-symbol' , '-type' are required after '-write-live-data'.");
                return false;
            }
            return true;
        }}
    },
    {keywords::_PROVIDER, rule{
        false,
        providers::binance,
        [](config& cfg, const char* args[],const int argc,size_t idx) -> bool {
            if (idx+1 >= argc || std::isdigit(static_cast<unsigned char>(args[idx + 1][0]))){
                throw std::runtime_error("Provider name not specified, delete '-provider' keyword to use value by default.");
                return false;
            }
            return providerLookup.find(static_cast<std::string>(args[idx+1])) != providerLookup.end();
        }
    }},
    {keywords::_SYMBOL, rule{
        true,
        "None",
        [](config& cfg, const char* args[],const int argc,size_t idx) -> bool {
            if (idx+1>=argc){
                throw std::runtime_error("'-symbol' keyword is used without a trading pair.");
                return false;
            }
            ///////std::string(args[idx+1]);
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
