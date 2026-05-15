#pragma once

#include <string>

#include "datasrc/endpoints/endpoints.hpp"
#include "connector/connector.hpp"
#include "filemanager/fileManager.hpp"

namespace oraculum {
    void makeOrderBookSnapshot(Config& cfg, Connector& connector, Provider& provider, std::string& symbol, FileManager& fm);
}

