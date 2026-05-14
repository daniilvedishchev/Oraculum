#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <string>
#include <unordered_set>

#include "filemanager/fileManager.hpp"
#include "datasrc/providers/providers.hpp"
#include "datasrc/resolvers/providerResolver.hpp"
#include "utils/utils.hpp"

namespace oraculum {

class CacheService {
private:
    FileManager& fileManager_;
public:
    explicit CacheService(FileManager& fileManager);

    void updateSymbols(Provider provider);
    bool isSymbolValidFromCache(Provider provider, const std::string& symbol);

    std::unordered_set<std::string> readSymbols(Provider provider);
    std::unordered_set<std::string> loadOrUpdateSymbols(Provider provider);
};

} // namespace oraculum
