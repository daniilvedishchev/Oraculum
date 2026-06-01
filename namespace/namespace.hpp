#pragma once

#include <string>
#include <variant>
#include <unordered_set>
#include <unordered_map>

struct MetaData;
namespace oraculum {
    using SymbolToMetadata = std::unordered_map<std::string,MetaData>;
}
