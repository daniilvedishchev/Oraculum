#pragma once

#include <string>
#include <variant>
#include <unordered_set>
#include <unordered_map>

#include "providers/providers.hpp"

namespace oraculum {

using RuleValue = std::variant<bool, double, int, std::string, Provider>;
using METADATA = std::unordered_map<std::string,std::unordered_set<std::string>>;
}
