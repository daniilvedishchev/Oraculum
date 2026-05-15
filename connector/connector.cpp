#include "connector/connector.hpp"

namespace oraculum {
    Connector::Connector() = default;

    cpr::Response Connector::request(const Provider& provider,const Connection& connectionType ,const std::string& endpoint) {
        const std::string url = resolveProviderBaseUrlOrThrow(provider, connectionType) + endpoint;
        const auto response = cpr::Get(cpr::Url{url});

        if (response.status_code != 200) {
            throw std::runtime_error(
                "Request failed. Status: " + std::to_string(response.status_code) + " URL: " + url
            );
        }
        
        return response;
    }
}
