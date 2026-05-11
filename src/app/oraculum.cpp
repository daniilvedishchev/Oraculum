#include <exception>
#include <iostream>
#include <string>
#include <unordered_set>

#include "cacheservice/CacheService.hpp"
#include "cli/validator.hpp"
#include "filemanager/fileManager.hpp"
#include "cli/cli.hpp"

int main(int argc, char* argv[]) {
    try {
        CLI cli = CLI(argc,argv);
        const Config cfg = cli.parseCliArgs();

        if (!cfg.writeLiveData) {
            return 0;
        }
        // Make it as an Internal step!!!
        const oraculum::Provider provider = resolveProvider(cfg.provider);

        oraculum::FileManager fileManager;
        oraculum::CacheService cacheService(fileManager);
        const std::unordered_set<std::string> symbols = cacheService.loadOrUpdateSymbols(provider);

        validateSymbolWithCache(cfg.symbol, symbols);
        fileManager.createFile(cfg.symbol, cfg.type);

        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
