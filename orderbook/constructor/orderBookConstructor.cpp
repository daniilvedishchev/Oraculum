#include "orderbook/constructor/orderBookConstructor.hpp"

namespace oraculum{
    OrderBookConstructor::OrderBookConstructor(Config& cfg) : DEPTH_(cfg.depth.value()), SYMBOL_(cfg.symbol) {}
    
}