#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <string>
#include <unordered_set>

#include "filemanager/fileManager.hpp"
#include "datasrc/providers/providers.hpp"
#include "datasrc/resolvers/providerResolver.hpp"
#include "datasrc/binance/urls/binanceBaseUrls.hpp" 

namespace oraculum {
    class Connector {
        private:
            /* data */
        public:
            Connector(/* args */);
            cpr::Response request(Provider provider, Connection connectionType);
        };
}