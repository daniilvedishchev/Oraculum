#include "socket/oraculumSocket/oraculumSocket.hpp"
namespace oraculum {
    OraculumSocket::OraculumSocket(const std::string& providerName, const std::string& connectorUrl) 
    : socket_{},connectorUrl_(connectorUrl), buffer_(1024) {
        socket_.setUrl(connectorUrl_);
        socket_.setOnMessageCallback(OnReceivedMessageCallback);
    }
}
