#pragma once

#include <string>

#include "datasrc/providers/providers.hpp"

namespace oraculum {
    Provider resolveProviderOrThrow(const std::string& providerName);
    const std::string& resolveProviderUrlOrThrow(Provider provider, Connection connectionType);
}
