#pragma once
#include <unordered_map>
#include <string>

enum class keywords {
    _WRITELIVEDATA,
    _PROVIDER,
    _SYMBOL,
    _TYPE
};

inline std::unordered_map<std::string, keywords> keywordlookup = {
    {"-write-live-data",keywords::_WRITELIVEDATA},
    {"-provider", keywords::_PROVIDER},
    {"-symbol", keywords::_SYMBOL},
    {"-type", keywords::_TYPE}
};