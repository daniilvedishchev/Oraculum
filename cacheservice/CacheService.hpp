#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <string>
#include <unordered_set>

#include "filemanager/fileManager.hpp"
#include "connector/builder/builder.hpp"
#include "datasrc/providers/providers.hpp"
#include "datasrc/resolvers/providerResolver.hpp"
#include "connector/retry/retry.hpp"
#include "utils/utils.hpp"
#include "namespace/namespace.hpp"
#include "cacheservice/meta/metadata.hpp"

namespace oraculum {

class CacheService {
private:
    FileManager& fileManager_;
    SymbolToMetadata symbolsMetadata;
public:
    explicit CacheService(FileManager& fileManager);

    void updateSymbols(Provider provider);
    bool isSymbolValidFromCache(Provider provider, const std::string& symbol);

    SymbolToMetadata readSymbolsMetadata(Provider provider);
    SymbolToMetadata loadOrUpdateSymbols(Provider provider);

    MetaData getMetaBySymbolOrThrow(std::string& symbol);
};

} // namespace oraculum
