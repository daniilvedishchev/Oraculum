#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <vector>

struct DepthUpdate {
    long long firstUpdateId;
    long long lastUpdateId;
    std::vector<std::vector<std::string>> bids;
    std::vector<std::vector<std::string>> asks;
    std::string raw;
};