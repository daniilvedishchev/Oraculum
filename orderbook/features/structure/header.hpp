#pragma once

#include <string>

inline const std::string featuresHeader =
    "ts_local_ms,ts_provider_ms,latency_ms,spread_ticks,best_ask,best_bid,mid,"
    "microprice_l1,relative_microprice_bps,"
    "imb_10,imb_20,imb_50,imb_100,imb_200,imb_500,imb_1000,"
    "imb_1bps,imb_2bps,imb_5bps,imb_10bps,imb_20bps,imb_50bps,imb_100bps";

inline const std::string liquidationsHeader =
    "event_time_ms,trade_time_ms,side,price_ticks,quantity_steps,avg_price,quantity,notional";
