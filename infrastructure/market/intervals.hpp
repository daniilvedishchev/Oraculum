#pragma once

#include <unordered_set>
#include <string>

namespace oraculum {
    std::unordered_set<std::string> intervals = {
        "1s",
        "1m", "3m", "5m", "15m", "30m",
        "1h", "2h", "4h", "6h", "8h", "12h",
        "1d", "3d",
        "1w",
        "1M"
    };
}