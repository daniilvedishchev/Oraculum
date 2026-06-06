#include "orderbook/features/FeatureEngine.hpp"

namespace {
double getOrZero(const std::unordered_map<int32_t, double>& values, int32_t key) {
    const auto it = values.find(key);
    return (it == values.end()) ? 0.0 : it->second;
}
} // namespace

FeatureEngine::FeatureEngine(LocalOrderBook& orderbook, oraculum::FileHandle& features): orderbook_(orderbook), features_(features){
    active_second = -1;
}

int64_t FeatureEngine::timestampSinceUNIX(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int64_t FeatureEngine::bestAsk(){
    return orderbook_.asks.begin()->first;
}

int64_t FeatureEngine::bestBid(){
    return orderbook_.bids.begin()->first;
}

double FeatureEngine::mid(){
    return static_cast<double>(bestAsk()+bestBid())/2.0;
}

int64_t FeatureEngine::spread_ticks(){
    return (bestAsk()-bestBid());
}

double FeatureEngine::microprice_l1(){
    int64_t askQty = orderbook_.asks.begin()->second;
    int64_t bidQty = orderbook_.bids.begin()->second;
    return static_cast<double>(bestBid()*bidQty + bestAsk()*askQty)/static_cast<double>(bidQty+askQty);
}

double FeatureEngine::relative_microprice(){
    auto middle = mid();
    return (microprice_l1()-middle)/middle;
}

double normalizeBps(int64_t bps){
    return bps/10000.0;
}

template<typename type>
type sortUnique(type& array){
    std::sort(array.begin(),array.end());
    array.erase(std::unique(array.begin(), array.end()), array.end());
    return array;
}

std::unordered_map<int32_t,double> FeatureEngine::xBpsImbalance(std::vector<int32_t>& bps){
    
    std::unordered_map<int32_t,double> xBpsPriceBandImbalance;
    std::vector<int32_t> sortedBps = std::move(sortUnique(bps));

    if (sortedBps.empty()) return xBpsPriceBandImbalance;

    int64_t midTicks = mid();

    for (int32_t& b : sortedBps){
    
        int64_t lowerTick = (midTicks * (10000 - b)) / 10000;
        int64_t upperTick = (midTicks * (10000 + b) + 9999) / 10000;

        int64_t qtyAsk = 0, qtyBid = 0;
        for (const auto& bid : orderbook_.bids){
            bool bidInBand = (bid.first <= upperTick) && (lowerTick <= bid.first);
            if (bidInBand){
                qtyBid += bid.second;
            } else break;
        }

        for (const auto& ask : orderbook_.asks){
            bool askInBand = (ask.first <= upperTick) && (lowerTick <= ask.first);
            if (askInBand){
                qtyAsk += ask.second;
            } else break;
        }
        double denom = static_cast<double>(qtyBid + qtyAsk);
        xBpsPriceBandImbalance[b] = (denom == 0.0) ? 0.0 : static_cast<double>(qtyBid - qtyAsk) / denom;
    }
    return xBpsPriceBandImbalance;
}

std::unordered_map<int32_t,double> FeatureEngine::nImbalance(std::vector<int32_t>& ns){
    std::unordered_map<int32_t,double> nLevelImbalance;
    if (ns.empty()) return nLevelImbalance;

    std::vector<int32_t> sortedNs = std::move(sortUnique(ns));

    auto itBids = orderbook_.bids.begin();
    auto itAsks = orderbook_.asks.begin();
    auto itN = sortedNs.begin();

    if (itN == sortedNs.end()) return nLevelImbalance;

    const size_t levels = std::min({static_cast<size_t>(sortedNs.back()), orderbook_.bids.size(), orderbook_.asks.size()});
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

FeatureRow FeatureEngine::makeFeatureRow(DepthUpdate& update){
    auto nImbalanceArray = nImbalance(nDepth);
    auto xBpsImbalanceArray = xBpsImbalance(xBps);

    FeatureRow row = FeatureRow{
        .ts_local_ms = timestampSinceUNIX(),
        .ts_provider_ms = update.lastUpdateTs,
        .spread_ticks = spread_ticks(),
        .best_ask = bestAsk(),
        .best_bid = bestBid(),
        .mid = mid(),
        .microprice_l1 = microprice_l1(),
        .relative_microprice_bps = relative_microprice() * 10000,
        .imb_10 = getOrZero(nImbalanceArray, 10),
        .imb_20 = getOrZero(nImbalanceArray, 20),
        .imb_50 = getOrZero(nImbalanceArray, 50),
        .imb_100 = getOrZero(nImbalanceArray, 100),
        .imb_200 = getOrZero(nImbalanceArray, 200),
        .imb_500 = getOrZero(nImbalanceArray, 500),
        .imb_1000 = getOrZero(nImbalanceArray, 1000),
        .imb_1bps = getOrZero(xBpsImbalanceArray, 1),
        .imb_2bps = getOrZero(xBpsImbalanceArray, 2),
        .imb_5bps = getOrZero(xBpsImbalanceArray, 5),
        .imb_10bps = getOrZero(xBpsImbalanceArray, 10),
        .imb_20bps = getOrZero(xBpsImbalanceArray, 20),
        .imb_50bps = getOrZero(xBpsImbalanceArray, 50),
        .imb_100bps = getOrZero(xBpsImbalanceArray, 100)
    };
    return row;
}

void FeatureEngine::features_1s(DepthUpdate& update){
    int64_t update_second = update.lastUpdateTs/MS_TO_SECONDS;
    if (active_second < 0) {
        active_second = update_second;
        point_second = makeFeatureRow(update);
        return;
    }

    if (active_second == update_second){
        point_second = makeFeatureRow(update);
    }

    if (update_second>active_second){
        features_.writeLine(toCsv(point_second));
        point_second = makeFeatureRow(update);
        active_second = update_second;
    }
}

std::string FeatureEngine::toCsv(const FeatureRow& r) {
    std::ostringstream oss;
    oss << r.ts_local_ms << ','
        << r.ts_provider_ms << ','
        << r.ts_local_ms - r.ts_provider_ms << ','
        << r.spread_ticks << ','
        << r.best_ask << ','
        << r.best_bid << ','
        << r.mid << ','
        << r.microprice_l1 << ','
        << r.relative_microprice_bps << ','
        << r.imb_10 << ',' << r.imb_20 << ',' << r.imb_50 << ','
        << r.imb_100 << ',' << r.imb_200 << ',' << r.imb_500 << ',' << r.imb_1000 << ','
        << r.imb_1bps << ',' << r.imb_2bps << ',' << r.imb_5bps << ','
        << r.imb_10bps << ',' << r.imb_20bps << ',' << r.imb_50bps << ',' << r.imb_100bps;
    return oss.str();
}
