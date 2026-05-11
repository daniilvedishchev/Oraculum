#pragma once

#include <string>
#include <unordered_map>
#include <variant>

#include "datasrc/binance/streamTypes/parameterizedStreamTypes.hpp"
#include "datasrc/binance/streamTypes/parameterlessStreamTypes.hpp"

namespace oraculum {

    using StreamTypeVariant = std::variant<parameterizedStreamType,parameterlessStreamType>;

    inline const std::unordered_map<std::string, StreamTypeVariant> kStreamTypeLookup = {
        {"depth", parameterizedStreamType::depth},
        {"partialDepth", parameterizedStreamType::partialDepth},
        {"kline", parameterizedStreamType::kline},
        {"rollingTicker", parameterizedStreamType::rollingTicker},
        {"trade", parameterlessStreamType::trade},
        {"aggTrade", parameterlessStreamType::aggTrade},
        {"bookTicker", parameterlessStreamType::bookTicker},
        {"avgPrice", parameterlessStreamType::avgPrice},
        {"ticker", parameterlessStreamType::ticker},
        {"miniTicker", parameterlessStreamType::miniTicker}
    };

}