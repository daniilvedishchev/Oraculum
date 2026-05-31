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

std::unordered_map<std::string,std::unordered_set<std::string>> CacheService::readSymbolsMetadata(Provider provider) {
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

    std::unordered_map<std::string,std::unordered_set<std::string>> metaData;
    std::string line;
    while (std::getline(file, line)) {

        const auto commaPosAfterSymbol = line.find(',');
        const auto commaPosAfterTickSize = line.find(',',commaPosAfterSymbol+1);
        const auto endlPos = line.find("\n");

        const std::string symbol = (commaPosAfterSymbol == std::string::npos) ? line : line.substr(0, commaPosAfterSymbol);
        const std::string tickSize = line.substr(commaPosAfterSymbol,commaPosAfterTickSize);
        const std::string stepSize = line.substr(commaPosAfterTickSize,endlPos);

        if (!symbol.empty()) {
            metaData[symbol] = {tickSize,stepSize};
        }
    }

    return metaData;
}

std::unordered_set<std::string> CacheService::loadOrUpdateSymbols(Provider provider) {
    std::unordered_map<std::string,std::unordered_set<std::string>> symbolsMeta = readSymbolsMetadata(provider);
    if (!symbolsMeta.empty()) {
        return symbols;
    }

    updateSymbols(provider);
    return readSymbolsMetadata(provider);
}

bool CacheService::isSymbolValidFromCache(Provider provider, const std::string& symbol) {
    const std::unordered_set<std::string> symbols = loadOrUpdateSymbols(provider);
    if (symbols.empty()) {
        return false;
    }

    const std::string normalizedSymbol = toUpper(symbol);
    return symbols.find(normalizedSymbol) != symbols.end();
}

} // namespace oraculum
