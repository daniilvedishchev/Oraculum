#pragma once
#include <unordered_map>
#include <vector>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <deque>

#include "namespace/namespace.hpp"
#include "orderbook/depth/depthUpdate.hpp"
#include "orderbook/localbook/LocalOrderBook.hpp"
#include "orderbook/features/structure/featureStructure.hpp"
#include "orderbook/features/structure/xBps.hpp"
#include "orderbook/features/structure/nDepth.hpp"
#include "orderbook/features/structure/featurePoint.hpp"
#include "filemanager/fileManager.hpp"
#include "time/time.hpp"
class FeatureEngine {
private:
    LocalOrderBook& orderbook_;
    std::deque<FeatureRow> history_;
    oraculum::FileHandle& features_;

    std::unordered_map<int32_t,double> nImbalance(std::vector<int32_t>& ns);
    std::unordered_map<int32_t,double> xBpsImbalance(std::vector<int32_t>& bps);

    int64_t bestAsk();
    int64_t bestBid();
    int64_t spread_ticks();
    int64_t timestampSinceUNIX();

    int64_t active_second;
    FeatureRow point_second;

    double microprice_l1();
    double relative_microprice();
    double mid();
    std::string toCsv(const FeatureRow& r);
    FeatureRow makeFeatureRow(DepthUpdate& update);

public:
    FeatureEngine(LocalOrderBook& orderbook,oraculum::FileHandle& features);
    void computeFeaturesCheckSecond(DepthUpdate& update);
    void features_1s(DepthUpdate& update);
};
