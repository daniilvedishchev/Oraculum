#include "cacheservice/CacheService.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <unordered_set>

#include "providers/binance/URLs.hpp"
#include "filemanager/FileManager.hpp"

namespace {
std::string toUpper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return value;
}
} // namespace

CacheService::CacheService(providers provider ,FileManager& fileManager) : _fileManager(fileManager) {
    (void) provider;
}

cpr::Response CacheService::_request(providers provider, const std::string& endpoint) {
    return _request(provider, parseEndpoint(endpoint));
}

cpr::Response CacheService::_request(providers provider, providerEndpoint endpoint) {
    if (endpoint == providerEndpoint::websocket) {
        throw std::runtime_error("WebSocket URL cannot be requested via HTTP GET. Use socket client.");
    }

    const std::string& url = resolveProviderUrl(provider, endpoint);
    const auto response = cpr::Get(cpr::Url{url});

    if (response.status_code != 200) {
        throw std::runtime_error(
            "Request failed. Status: " + std::to_string(response.status_code) + " URL: " + url
        );
    }

    return response;
}



void CacheService::updateSymbols(providers& provider){
    std::string providerStr = providerStrLookup.find(provider)->second;

    if (!_fileManager.dirExistsInLocalOraculumEnv("cache/"+providerStr)){
       std::filesystem::create_directories(_fileManager.envPath()/"cache"/providerStr);
    }

    cpr::Response response = _request(provider,providerEndpoint::symbols);
    file f = _fileManager.createFile("cache/"+providerStr+"/symbols",true);
    nlohmann::json data = nlohmann::json::parse(response.text);

    for (const auto& tradingpair : data["symbols"]){
        f.write(tradingpair["symbol"].get<std::string>());
    }

}

std::unordered_set<std::string> CacheService::readSymbols(providers provider) {
    const auto providerStrIt = providerStrLookup.find(provider);
    if (providerStrIt == providerStrLookup.end()) {
        throw std::runtime_error("Unknown provider for symbols cache.");
    }

    const std::filesystem::path symbolCachePath =
        _fileManager.envPath() / "cache" / providerStrIt->second / "symbols";

    if (!std::filesystem::exists(symbolCachePath)) {
        return {};
    }

    std::ifstream in(symbolCachePath);
    if (!in) {
        throw std::runtime_error("Cannot open symbols cache file: " + symbolCachePath.string());
    }

    std::unordered_set<std::string> symbols;
    std::string symbol;
    while (std::getline(in, symbol)) {
        if (!symbol.empty()) {
            symbols.insert(symbol);
        }
    }

    return symbols;
}

bool CacheService::isSymbolValidFromCache(providers provider, const std::string& symbol) {
    const std::unordered_set<std::string> symbols = readSymbols(provider);
    if (symbols.empty()) {
        return false;
    }

    const std::string normalizedSymbol = toUpper(symbol);
    return symbols.find(normalizedSymbol) != symbols.end();
}
