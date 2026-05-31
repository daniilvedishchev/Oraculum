#include "cacheservice/CacheService.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <unordered_set>
namespace oraculum {

CacheService::CacheService(FileManager& fileManager) : fileManager_(fileManager){}

void CacheService::updateSymbols(Provider provider) {
    const auto providerNameIt = kProviderToString.find(provider);
    if (providerNameIt == kProviderToString.end()) {
        throw std::runtime_error("Unknown provider for symbols update.");
    }
    const std::string& providerName = providerNameIt->second;

    if (!fileManager_.directoryExistsInOraculumEnv("cache/" + providerName)) {
        std::filesystem::create_directories(fileManager_.environmentPath() / "cache" / providerName);
    }
    
    std::string url = buildUrl(provider,Connection::Symbols,"");
    const cpr::Response response = requestRetryOrThrow(url);

    FileHandle fileHandle = fileManager_.createFile("cache/" + providerName + "/symbolsMeta", true);
    const nlohmann::json data = nlohmann::json::parse(response.text);

    for (const auto& tradingPair : data["symbols"]) {
        std::string cacheRow = toUpper(tradingPair["symbol"].get<std::string>()) 
        + "," 
        + tradingPair["filters"][0]["tickSize"].get<std::string>()
        + ","
        + tradingPair["filters"][1]["stepSize"].get<std::string>();
        fileHandle.writeLine(cacheRow);
    }
}

SymbolToMetadata CacheService::readSymbolsMetadata(Provider provider) {
    const auto providerNameIt = kProviderToString.find(provider);
    if (providerNameIt == kProviderToString.end()) {
        throw std::runtime_error("Unknown provider for symbols cache.");
    }

    const std::filesystem::path symbolCachePath =
        fileManager_.environmentPath() / "cache" / providerNameIt->second / "symbolsMeta";

    if (!std::filesystem::exists(symbolCachePath)) {
        return {};
    }

    std::ifstream file(symbolCachePath);
    if (!file) {
        throw std::runtime_error("Cannot open symbols cache file: " + symbolCachePath.string());
    }

    SymbolToMetadata metaData;
    std::string line;
    while (std::getline(file, line)) {

        const auto commaPosAfterSymbol = line.find(',');
        const auto commaPosAfterTickSize = line.find(',',commaPosAfterSymbol+1);
        const auto endlPos = line.find("\n");

        const std::string symbol = (commaPosAfterSymbol == std::string::npos) ? line : line.substr(0, commaPosAfterSymbol);
        const std::string tickSize = line.substr(commaPosAfterSymbol,commaPosAfterTickSize);
        const std::string stepSize = line.substr(commaPosAfterTickSize,endlPos);

        if (!symbol.empty()) {
            metaData[symbol] = MetaData{.tickSize = std::stod(tickSize),.stepSize = std::stod(tickSize)};
        }
    }

    return metaData;
}

SymbolToMetadata CacheService::loadOrUpdateSymbols(Provider provider) {
    SymbolToMetadata symbolsMeta = readSymbolsMetadata(provider);
    if (!symbolsMeta.empty()) {
        symbolsMetadata = std::move(symbolsMeta);
        return symbolsMeta;
    }

    updateSymbols(provider);
    return readSymbolsMetadata(provider);
}

bool CacheService::isSymbolValidFromCache(Provider provider, const std::string& symbol) {
    SymbolToMetadata symbols = loadOrUpdateSymbols(provider);
    if (symbols.empty()) {
        return false;
    }

    const std::string normalizedSymbol = toUpper(symbol);
    return symbols.find(normalizedSymbol) != symbols.end();
}

MetaData CacheService::getMetaBySymbolOrThrow(std::string& symbol){
    auto it = symbolsMetadata.find(symbol);
    
    if (it == symbolsMetadata.end()){
        throw std::runtime_error("No existing metadata for symbol: " + symbol + "\n");
    }
    return it->second;
}

} // namespace oraculum
