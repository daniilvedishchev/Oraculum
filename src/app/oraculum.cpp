#include <iostream>
#include <string>

#include "filemanager/FileManager.hpp"
#include "cacheservice/CacheService.hpp"

int main(int argc,char* argv[]) {
    FileManager fm = FileManager();
    providers provider = providers::binance;
    CacheService cache = CacheService(provider,fm);
    cache.updateSymbols(provider);
    return 0;
}