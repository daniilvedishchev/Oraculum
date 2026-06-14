#include "socket/oraculumSocket/oraculumSocket.hpp"

#include "orderbook/depth/depthUpdate.hpp"
#include "trades/liquidations/liquidations.hpp"
#include "trades/structure/trades.h"

namespace oraculum {
    template<typename BufferItem>
    OraculumSocket<BufferItem>::OraculumSocket(const std::string& providerName, const std::string& connectorUrl, socketCallback& onMsgCallback) 
    : socket_{},connectorUrl_(connectorUrl),buffer_(),callback_(onMsgCallback){
        socket_.setUrl(connectorUrl_);
        socket_.setOnMessageCallback(callback_);
    }

    template class OraculumSocket<DepthUpdate>;
    template class OraculumSocket<AggregateTrade>;
    template class OraculumSocket<Liquidation>;
}
