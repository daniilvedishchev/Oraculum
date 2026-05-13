#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <string>
#include <unordered_set>

#include "datasrc/providers/providers.hpp"

namespace oraculum {

class FileManager;

class CacheService {
private:
    FileManager& fileManager_;

    cpr::Response request(Provider provider, Connection connection);

public:
    explicit CacheService(FileManager& fileManager);

    void updateSymbols(Provider provider);
    std::unordered_set<std::string> readSymbols(Provider provider);
    std::unordered_set<std::string> loadOrUpdateSymbols(Provider provider);
    bool isSymbolValidFromCache(Provider provider, const std::string& symbol);
};

} // namespace oraculum
