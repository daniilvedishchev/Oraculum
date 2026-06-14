#pragma once

#include <string>

namespace oraculum {
    enum class Connection {
        Api,
        WebSocket,
        FuturesMarketWebSocket,
        Symbols
    };

    struct ProviderBase {
        std::string apiBaseUrl;
        std::string websocketBaseUrl;
        std::string futuresMarketWebSocketBaseUrl;
        std::string symbolsEndpoint;
    };
}
