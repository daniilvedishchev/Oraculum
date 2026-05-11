#pragma once

#include <string>

enum class endpoint {
    Api,
    WebSocket,
    Symbols
};

struct providerEndpoints {
    std::string apiBaseUrl;
    std::string websocketBaseUrl;
    std::string symbolsEndpoint;
};