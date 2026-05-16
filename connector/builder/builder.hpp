#pragma once

#include <string>

#include "datasrc/resolvers/providerResolver.hpp"

namespace oraculum {
    inline const std::string buildUrl(const Provider& provider, const Connection& connectionType, const std::string& endpoint){
        return resolveProviderBaseUrlOrThrow(provider, connectionType) + endpoint;
    }
}