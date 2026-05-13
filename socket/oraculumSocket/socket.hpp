#pragma once

#include <iostream>
#include <string>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include "socket/callbacks/callback.hpp"

class OraculumSocket {
private:
    std::string connectorUrl_;
    ix::WebSocket socket_;
public:
    OraculumSocket(const std::string& providerName, const std::string& connectorUrl);
};
