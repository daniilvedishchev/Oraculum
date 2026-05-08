#pragma once
#include <unordered_map>
#include <string>

enum class providers {
    binance
};

inline std::unordered_map<std::string, providers> providerlookup = {
    {"binance",providers::binance}
};