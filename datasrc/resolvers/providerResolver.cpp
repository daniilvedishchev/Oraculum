#include "datasrc/resolvers/providerResolver.hpp"

#include <stdexcept>

#include "utils/utils.hpp"
#include "datasrc/providers/baseUrl.hpp"
#include "datasrc/providers/providers.hpp"

namespace oraculum {

    Provider resolveProviderOrThrow(const std::string& providerName) {
        const std::string normalizedProvider = toLower(providerName);
        const auto providerIt = kStringToProvider.find(normalizedProvider);
        if (providerIt == kStringToProvider.end()) {
            throw std::runtime_error("Unsupported provider: " + providerName);
        }

        return providerIt->second;
    }
    const std::string& resolveConnectionBaseUrl (const ProviderBase& urls, const Connection& connectionType){
        switch (connectionType) {
            case Connection::Api:
                return urls.apiBaseUrl;
            case Connection::WebSocket:
                return urls.websocketBaseUrl;
            case Connection::FuturesMarketWebSocket:
                return urls.futuresMarketWebSocketBaseUrl;
            case Connection::Symbols:
                return urls.symbolsEndpoint;
        }
        throw std::runtime_error("Unsupported base selector.");
    }

    const std::string& resolveProviderBaseUrlOrThrow(const Provider& provider, const Connection& connectionType) {
        auto providerIt = kProviderToUrls.find(provider);
        if (providerIt == kProviderToUrls.end()) {
            throw std::runtime_error("This provider is not supported.");
        }
        const ProviderBase& urls = providerIt->second;
        return resolveConnectionBaseUrl(urls,connectionType);
    }

} // namespace oraculum
