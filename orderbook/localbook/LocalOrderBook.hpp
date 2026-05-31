#pragma once
#include <map>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include "orderbook/depth/depthUpdate.hpp"

class LocalOrderBook {
private:
    nlohmann::json SNAPSHOT_;

    const double PRICE_TICKS;
    const double QTY_TICKS;

    template<typename Compare>
    void extractPriceQtyTicksFromLvl(std::map<int64_t,int64_t,Compare>& container, const std::vector<std::vector<std::string>>& levels);
    int64_t priceQtyTicks(const std::string& input,const bool qty = false);
public:
    std::map<int64_t, int64_t, std::greater<>> bids;
    std::map<int64_t, int64_t, std::less<>> asks;

    long long LAST_UPDATE_ID;

    void applyUpdate(const DepthUpdate& update);

    LocalOrderBook(nlohmann::json SNAPSHOT, const double tickSize, const double stepSize);
};


#include "orderbook/localbook/LocalOrderBook.tpp"
