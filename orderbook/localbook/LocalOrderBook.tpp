#pragma once
#include "orderbook/localbook/LocalOrderBook.hpp"

template<typename Compare>
void LocalOrderBook::extractPriceQtyTicksFromLvl(std::map<int64_t,int64_t,Compare>& book, const std::vector<std::vector<std::string>>& levels){
    for (const auto& level : levels){
        int64_t PRICE_IN_TICKS = priceQtyTicks(level[0]);
        int64_t QTY_IN_TICKS = priceQtyTicks(level[1],true);
        if (QTY_IN_TICKS == 0 && book.find(PRICE_IN_TICKS) != book.end()){
            book.erase(PRICE_IN_TICKS);
            continue;
        }
        book.insert_or_assign(PRICE_IN_TICKS,QTY_IN_TICKS);
    }
}