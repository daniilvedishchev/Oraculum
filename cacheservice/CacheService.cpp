#include "cacheservice/CacheService.hpp"

#include <stdexcept>

#include "providers/binance/URLs.hpp"
#include "filemanager/FileManager.hpp"

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
