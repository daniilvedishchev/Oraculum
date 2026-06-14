#pragma once

#include <cstdint>
#include <string>

const std::string aggregatedTradesHeader = "event_time_ms,trade_time_ms, \
latency_ms,aggregate_trade_id, \
price,quantity,is_buyer_maker";

struct AggregateTrade {
    int64_t event_time_ms;
    int64_t trade_time_ms;
    int64_t latency_ms;

    int64_t aggregate_trade_id;

    int64_t price;
    int64_t quantity;

    bool is_buyer_maker;
};