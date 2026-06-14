#include "socket/oraculumSocket/oraculumSocket.hpp"
namespace oraculum {
    template<typename BufferItem>
    OraculumSocket<BufferItem>::OraculumSocket(const std::string& providerName, const std::string& connectorUrl, socketCallback& onMsgCallback) 
    : socket_{},connectorUrl_(connectorUrl),buffer_(),callback_(onMsgCallback){
        socket_.setUrl(connectorUrl_);
        socket_.setOnMessageCallback(callback_);
    }
}
