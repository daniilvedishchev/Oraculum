#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <string>
#include <unordered_set>

#include "providers/providers.hpp"

class FileManager;

class CacheService {
    private:
        void _updateCache();
        void _createCache();

        FileManager& _fileManager;

        cpr::Response _request(providers provider, providerEndpoint endpoint);
        cpr::Response _request(providers provider, const std::string& endpoint);

    public:
        explicit CacheService(providers provider,FileManager& fileManager);
        void updateSymbols(providers& provider);
        std::unordered_set<std::string> readSymbols(providers provider);
        bool isSymbolValidFromCache(providers provider, const std::string& symbol);
};
