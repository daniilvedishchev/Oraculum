#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include "datasrc/endpoints/endpoints.hpp"
#include "connector/retry/retry.hpp"
#include "connector/builder/builder.hpp"
#include "filemanager/fileManager.hpp"

namespace oraculum {
    void makeOrderBookSnapshot(const Config& cfg, const Provider& provider, const std::string& symbol, const FileManager& fm);
}

