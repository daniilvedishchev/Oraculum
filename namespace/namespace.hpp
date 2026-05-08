#pragma once

#include <string>
#include <variant>

#include "providers/providers.hpp"

namespace Oraculum {
    using value = std::variant<bool, double, int, std::string, providers>;
}