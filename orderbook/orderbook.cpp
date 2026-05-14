#include "orderbook/orderbook.hpp"
#include "connector/connector.cpp"
#include "filemanager/fileManager.cpp"

namespace oraculum {
    void makeOrderBookSnapshot(Provider& provider,Connector& connector,FileManager& fm){
        cpr::Response response = connector.request(provider,Connection::Api);
        
        
    }
}