#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "cacheservice/CacheService.hpp"
#include "filemanager/fileManager.hpp"
#include "cli/cli.hpp"
#include "utils/utils.hpp"
#include "datasrc/binance/streamTypes/allStreamTypes.hpp"
#include "datasrc/resolvers/providerResolver.hpp"

int main(int argc, char* argv[]) {
    try {
        CLI cli = CLI(argc,argv);
        const Config cfg = cli.parseCliArgs();

        if (!cfg.writeLiveData) {
            return 0;
        }

        if (cfg.symbol.empty()) {
            throw std::runtime_error("Missing value for required argument: -symbol");
        }
        if (cfg.type.empty()) {
            throw std::runtime_error("Missing value for required argument: -type");
        }

        const oraculum::Provider provider = oraculum::resolveProviderOrThrow(cfg.provider);
        const std::string normalizedType = toLower(cfg.type);
        if (oraculum::kStreamTypeLookup.find(normalizedType) == oraculum::kStreamTypeLookup.end()) {
            throw std::runtime_error("Unsupported stream type: " + cfg.type);
        }

        oraculum::FileManager fileManager;
        oraculum::CacheService cacheService(fileManager);

        if (!cacheService.isSymbolValidFromCache(provider, cfg.symbol)) {
            throw std::runtime_error("Unknown symbol for selected provider: " + cfg.symbol);
        }
        fileManager.createFile(cfg.symbol, cfg.type);

        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
