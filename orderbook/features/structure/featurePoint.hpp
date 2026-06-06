#pragma once 
#include <cstdint>

struct featurePoint {
    int64_t ts_provider_ms;

    double mid;
    double microprice_l1;
    double relative_microprice_bps;
    int64_t spread_ticks;

    double imb_20;
    double imb_100;
    double imb_500;

    double imb_5bps;
    double imb_20bps;
    double imb_100bps;
};
