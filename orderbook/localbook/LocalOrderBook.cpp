#include "orderbook/localbook/LocalOrderBook.hpp"
#include "orderbook/features/FeatureEngine.hpp"

LocalOrderBook::LocalOrderBook(nlohmann::json SNAPSHOT): SNAPSHOT_(std::move(SNAPSHOT)), LAST_UPDATE_ID(SNAPSHOT_.at("last_update_id")) {
    extractPriceQtyTicksFromLvl(bids,SNAPSHOT_.at("snapshot").at("bids"));
    extractPriceQtyTicksFromLvl(asks,SNAPSHOT_.at("snapshot").at("asks"));
}

int64_t LocalOrderBook::priceQtyTicks(const std::string& input,const bool qty){
    long double TICK_COEFF{PRICE_TICKS};
    if (qty) TICK_COEFF = QTY_TICKS;
    return round(std::stold(input)/TICK_COEFF);
}

void LocalOrderBook::applyUpdate(const DepthUpdate& update){
    std::cerr << "[Oraculum] updating local orderbook."<<std::endl;
    std::cerr << "[Oraculum] Begin ID: " << update.firstUpdateId<<"End ID: " << update.lastUpdateId << std::endl;
    
    extractPriceQtyTicksFromLvl(bids,update.bids);
    extractPriceQtyTicksFromLvl(asks,update.asks);
    FeatureEngine featureEngine = FeatureEngine(*this);
    featureEngine.run();
    LAST_UPDATE_ID = update.lastUpdateId;
}
