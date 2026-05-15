#pragma once

#include <stdexcept>

#include "datasrc/resolvers/providerResolver.hpp"
#include "config/config.hpp"
#include "utils/utils.hpp"

namespace oraculum {
    inline std::string makeOrderBookSnapshotEndpoint(Config& cfg, Provider& provider, Connection& connection) {
        auto urls = kProviderToUrls.find(provider)->second;
        const std::string& base = resolveConnectionBaseUrl(urls,connection);
        if (provider == Provider::Binance){
            std::string level = (cfg.depth.has_value()) ? cfg.depth.value() : "20";
            return cfg.type + "?" + "symbol=" + toUpper(cfg.symbol) + "&" + "limit=" + level;
        }
        throw std::runtime_error("Orderbook snapshot not supported by this provider.");
    }

    inline std::string makeSymbolEndpoint(Config& cfg, Provider& provider) {
        auto urls = kProviderToUrls.find(provider)->second;
        const std::string& base = resolveConnectionBaseUrl(urls,Connection::Symbols);
        return base;
    }
}
