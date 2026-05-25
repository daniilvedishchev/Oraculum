#pragma once

#include <iostream>
#include <string>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include "socket/callbacks/callback.hpp"

#include "socket/ring/ringBuffer.hpp"

namespace oraculum {
    class OraculumSocket {
    private:
        std::string connectorUrl_;
    public:
        ix::WebSocket socket_;
        RingBuffer<DepthUpdate> orderBookUpdateBuffer_;
        OraculumSocket(const std::string& providerName, const std::string& connectorUrl);
    };
}

