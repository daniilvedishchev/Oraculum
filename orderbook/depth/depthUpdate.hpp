#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <vector>

struct DepthUpdate {
    std::string raw;

    long long firstUpdateId;
    long long lastUpdateId;
    
    long long lastUpdateTs;

    std::vector<std::vector<std::string>> bids;
    std::vector<std::vector<std::string>> asks;
};