#pragma once

#include <string>
#include <variant>

#include "providers/providers.hpp"

namespace oraculum {

using RuleValue = std::variant<bool, double, int, std::string, Provider>;

}
