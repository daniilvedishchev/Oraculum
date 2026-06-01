#pragma once
#include <unordered_map>
#include <vector>
#include <chrono>
#include <cstdint>
#include <sstream>
#include "orderbook/localbook/LocalOrderBook.hpp"
#include "orderbook/features/structure/featureStructure.hpp"
#include "orderbook/features/structure/xBps.hpp"
#include "orderbook/features/structure/nDepth.hpp"
class FeatureEngine {
private:
    LocalOrderBook& orderbook_;

    std::unordered_map<int32_t,double> nImbalance(std::vector<int32_t>& ns);
    std::unordered_map<int32_t,double> xBpsImbalance(std::vector<int32_t>& bps);

    int64_t bestAsk();
    int64_t bestBid();
    int64_t spread_ticks();
    int64_t timestampSinceUNIX();

    double microprice_l1();
    double relative_microprice();
    double mid();
public:
    FeatureEngine(LocalOrderBook& orderbook);
    void run();
    FeatureRow compute();
    std::string toCsv(const FeatureRow& r);
};
