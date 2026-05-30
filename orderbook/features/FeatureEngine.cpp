#include "orderbook/features/FeatureEngine.hpp"

FeatureEngine::FeatureEngine(LocalOrderBook& orderbook): orderbook_(orderbook) {}

int64_t FeatureEngine::timestampSinceUNIX(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void FeatureEngine::run(){
    const std::vector<size_t> ns = {1,5,20,50,100,1000};
    auto depthImbalanceMap = nImbalance(ns);
}

std::unordered_map<size_t,double> FeatureEngine::nImbalance(const std::vector<size_t>& ns){
    std::unordered_map<size_t,double> nLevelImbalance;
    if (ns.empty()) return nLevelImbalance;

    std::vector<size_t> sortedNs = ns;
    std::sort(sortedNs.begin(),sortedNs.end());
    sortedNs.erase(std::unique(sortedNs.begin(), sortedNs.end()), sortedNs.end());

    auto itBids = orderbook_.bids.begin();
    auto itAsks = orderbook_.asks.begin();
    auto itN = sortedNs.begin();

    if (itN == sortedNs.end()) return nLevelImbalance;

    const size_t levels = std::min({sortedNs.back(), orderbook_.bids.size(), orderbook_.asks.size()});
    int64_t cumSum{0}, cumDif{0};
   
    for (size_t level = 1; level <= levels; ++level,++itBids,++itAsks){
        auto askQty = itAsks->second;
        auto bidQty = itBids->second;

        cumSum += askQty + bidQty;
        cumDif += bidQty - askQty;

        if (level == *itN){
            double imbalance = (cumSum==0) ? 0 : static_cast<double>(cumDif)/static_cast<double>(cumSum);
            nLevelImbalance[*itN] = imbalance;
            itN++;
            std::cout<< "[Oraculum] Level: " << level << " Imbalance: " <<  imbalance <<" Timestamp: "<< timestampSinceUNIX() <<std::endl;
            if (itN == sortedNs.end()) break;
        }
    }
    return nLevelImbalance;
}