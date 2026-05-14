#pragma once
#include <unordered_map>
#include <string>
namespace oraculum {
    enum class Keyword {
        WriteLiveData,
        Provider,
        Symbol,
        Type,
        Depth,
        Speed,
        TimeFrame
    };

    inline const std::unordered_map<std::string, Keyword> kKeywordLookup = {
        {"-write-live-data", Keyword::WriteLiveData},
        {"-provider", Keyword::Provider},
        {"-symbol", Keyword::Symbol},
        {"-type", Keyword::Type},
        {"-d", Keyword::Depth},
        {"-s", Keyword::Speed},
        {"-tf", Keyword::TimeFrame}
    };
}
