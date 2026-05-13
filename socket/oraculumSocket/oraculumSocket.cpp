#include "socket/oraculumSocket/oraculumSocket.hpp"

OraculumSocket::OraculumSocket(const std::string& providerName, const std::string& connectorUrl) 
: socket_{},connectorUrl_(connectorUrl) {
    socket_.setUrl(connectorUrl_);
    socket_.setOnMessageCallback(OnReceivedMessageCallback);
    socket_.start();
}
