#pragma once

#include <string>
#include <variant>
#include <unordered_set>
#include <unordered_map>

#include "providers/providers.hpp"

struct MetaData;
namespace oraculum {

using RuleValue = std::variant<bool, double, int, std::string, Provider>;
using SymbolToMetadata = std::unordered_map<std::string,MetaData>;
using Metadata = std::unordered_set<std::string>;
}
