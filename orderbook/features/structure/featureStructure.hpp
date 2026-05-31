#pragma once
#include <cstdint>

struct FeatureRow {
    int64_t ts_local_ms;
    int64_t spread_ticks;
    int64_t best_ask;
    int64_t best_bid;
    int64_t mid;

    double microprice_l1;
    double relative_microprice;

    double imb_10;
    double imb_20;
    double imb_50;
    double imb_100;
    double imb_200;
    double imb_500;
    double imb_1000;

    double imb_1bps;
    double imb_2bps;
    double imb_5bps;
    double imb_10bps;
    double imb_20bps;
    double imb_50bps;
    double imb_100bps;
};
