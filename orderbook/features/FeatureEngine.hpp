#pragma once
#include <unordered_map>
#include <vector>
#include <chrono>
#include "orderbook/localbook/LocalOrderBook.hpp"

class FeatureEngine {
private:
    LocalOrderBook& orderbook_;
    std::unordered_map<size_t,double> nImbalance(const std::vector<size_t>& ns);
    int64_t timestampSinceUNIX();
public:
    FeatureEngine(LocalOrderBook& orderbook);
    void run();
};
