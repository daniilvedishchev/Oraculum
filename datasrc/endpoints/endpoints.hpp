#pragma once

#include <stdexcept>

#include "datasrc/resolvers/providerResolver.hpp"
#include "config/config.hpp"
#include "utils/utils.hpp"

namespace oraculum {
    inline const std::string makeOrderBookSnapshotEndpoint(const Config& cfg) {
        if (resolveProviderOrThrow(cfg.provider) == Provider::Binance){
            std::string level = (cfg.depth.has_value()) ? cfg.depth.value() : "20";
            return cfg.type + "?" + "symbol=" + toUpper(cfg.symbol) + "&" + "limit=" + level;
        }
        throw std::runtime_error("Orderbook snapshot not supported by this provider.");
    }

    inline const std::string makeOrderBookUpdateEndpoint(const Config& cfg) {
        if (resolveProviderOrThrow(cfg.provider) == Provider::Binance){
            std::string level = (cfg.depth.has_value()) ? cfg.depth.value() : "20";
            return toLower(cfg.symbol) + "@" + cfg.type + level;
        }
        throw std::runtime_error("Orderbook update not supported by this provider.");
    }

    inline std::string makeSymbolEndpoint(const Provider& provider) {
        auto urls = kProviderToUrls.find(provider)->second;
        const std::string& base = resolveConnectionBaseUrl(urls,Connection::Symbols);
        return base;
    }
}
