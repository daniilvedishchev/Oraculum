#pragma once

#include <unordered_map>
#include <string>

#include "providers/providers.hpp"
#include "providers/binance/binance.hpp"

inline std::unordered_map<providers,std::unordered_map<std::string,std::string>> URLs = {
    {providers::binance,_binance}
};