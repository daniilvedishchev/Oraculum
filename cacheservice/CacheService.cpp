#include "cacheservice/CacheService.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <unordered_set>
namespace oraculum {

CacheService::CacheService(FileManager& fileManager) : fileManager_(fileManager) {}

void CacheService::updateSymbols(Provider provider) {
    const auto providerNameIt = kProviderToString.find(provider);
    if (providerNameIt == kProviderToString.end()) {
        throw std::runtime_error("Unknown provider for symbols update.");
    }
    const std::string& providerName = providerNameIt->second;

    if (!fileManager_.directoryExistsInOraculumEnv("cache/" + providerName)) {
        std::filesystem::create_directories(fileManager_.environmentPath() / "cache" / providerName);
    }

    const cpr::Response response = request(provider, Connection::Symbols);
    FileHandle fileHandle = fileManager_.createFile("cache/" + providerName + "/symbols", true);
    const nlohmann::json data = nlohmann::json::parse(response.text);

    for (const auto& tradingPair : data["symbols"]) {
        fileHandle.writeLine(tradingPair["symbol"].get<std::string>());
    }
}

std::unordered_set<std::string> CacheService::readSymbols(Provider provider) {
    const auto providerNameIt = kProviderToString.find(provider);
    if (providerNameIt == kProviderToString.end()) {
        throw std::runtime_error("Unknown provider for symbols cache.");
    }

    const std::filesystem::path symbolCachePath =
        fileManager_.environmentPath() / "cache" / providerNameIt->second / "symbols";

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

std::unordered_set<std::string> CacheService::loadOrUpdateSymbols(Provider provider) {
    std::unordered_set<std::string> symbols = readSymbols(provider);
    if (!symbols.empty()) {
        return symbols;
    }

    updateSymbols(provider);
    return readSymbols(provider);
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
