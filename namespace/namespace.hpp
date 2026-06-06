#pragma once

#include <string>
#include <variant>
#include <unordered_set>
#include <unordered_map>
#include <vector>

struct MetaData;
namespace oraculum {
    using SymbolToMetadata = std::unordered_map<std::string,MetaData>;
    using Delta = std::vector<uint8_t>;
}
