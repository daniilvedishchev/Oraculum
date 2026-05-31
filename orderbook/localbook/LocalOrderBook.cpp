#include "orderbook/localbook/LocalOrderBook.hpp"

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
    extractPriceQtyTicksFromLvl(bids,update.bids);
    extractPriceQtyTicksFromLvl(asks,update.asks);
    LAST_UPDATE_ID = update.lastUpdateId;
}
