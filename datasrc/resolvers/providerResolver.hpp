#pragma once

#include <string>

#include "datasrc/providers/providers.hpp"

namespace oraculum {
    Provider resolveProviderOrThrow(const std::string& providerName);
    const std::string& resolveProviderBaseUrlOrThrow(const Provider& provider, const Connection& connectionType);
    const std::string& resolveConnectionBaseUrl (const ProviderBase& urls, const Connection& connectionType);
}
