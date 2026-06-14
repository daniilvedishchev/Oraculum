#pragma once

#include <cstdint>

enum class LiquidationSide {
    LongLiquidation,
    ShortLiquidation
};

struct Liquidation {
    int64_t event_time_ms;
    int64_t trade_time_ms;

    LiquidationSide side;

    int64_t price_ticks;
    int64_t quantity_steps;

    double avg_price;
    double quantity;
    double notional;
};
