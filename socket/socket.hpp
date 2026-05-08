#pragma once
#include <ixwebsocket/IXWebSocketServer.h>
#include <string>

class OraculumSocket {
    private:
        std::string connectorURL_;
        ix::WebSocket socket_;
    public:
        OraculumSocket(std::string provider);
};

