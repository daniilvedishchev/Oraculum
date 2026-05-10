#pragma once
#include <unordered_map>
#include <string>

enum class Keyword {
    WriteLiveData,
    Provider,
    Symbol,
    Type
};

inline const std::unordered_map<std::string, Keyword> kKeywordLookup = {
    {"-write-live-data", Keyword::WriteLiveData},
    {"-provider", Keyword::Provider},
    {"-symbol", Keyword::Symbol},
    {"-type", Keyword::Type}
};
