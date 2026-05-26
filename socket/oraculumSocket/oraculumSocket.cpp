#include "socket/oraculumSocket/oraculumSocket.hpp"
namespace oraculum {
    OraculumSocket::OraculumSocket(const std::string& providerName, const std::string& connectorUrl) 
    : socket_{},connectorUrl_(connectorUrl),orderBookUpdateBuffer_(){
        socket_.setUrl(connectorUrl_);
        socket_.setOnMessageCallback(OnReceivedMessageCallback);
    }
}
