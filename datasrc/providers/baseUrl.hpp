#pragma once

#include <string>

namespace oraculum {
    enum class Connection {
        Api,
        WebSocket,
        Symbols
    };

    struct ProviderBase {
        std::string apiBaseUrl;
        std::string websocketBaseUrl;
        std::string symbolsEndpoint;
    };
}