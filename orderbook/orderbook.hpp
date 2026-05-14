#pragma once

#include "datasrc/binance/urls/binanceBaseUrls.hpp"


class Connector;
class FileManager;
namespace oraculum {
    void makeOrderBookSnapshot(Provider& provider,Connector& connector, FileManager& fm);
}


