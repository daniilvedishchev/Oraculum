#pragma once

#include <iostream>
#include <string>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include "socket/callbacks/callback.hpp"

#include "socket/ring/ringBuffer.hpp"

using socketCallback = std::function<void(const ix::WebSocketMessagePtr&)>;
namespace oraculum {
    template<typename BufferItem>
    class OraculumSocket {
    private:
        std::string connectorUrl_;
        socketCallback& callback_;
    public:
        ix::WebSocket socket_;
        RingBuffer<BufferItem> buffer_;
        OraculumSocket(const std::string& providerName, const std::string& connectorUrl, socketCallback& onMsgCallback);
    };
}

