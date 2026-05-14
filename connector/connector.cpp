#include "connector/connector.hpp"
#include "datasrc/providers/providers.hpp"

namespace oraculum {
    cpr::Response Connector::request(Provider provider, Connection connectionType, Endpoint) {
        if (connectionType == Connection::WebSocket) {
            throw std::runtime_error("WebSocket URL cannot be requested via HTTP GET. Use socket client.");
        }

        const std::string& url = resolveProviderUrlOrThrow(provider,connectionType);
        const auto response = cpr::Get(cpr::Url{url});

        if (response.status_code != 200) {
            throw std::runtime_error(
                "Request failed. Status: " + std::to_string(response.status_code) + " URL: " + url
            );
        }
        
        return response;
    }
}