#pragma once

#include <unordered_set>
#include "keywords/keywords.hpp"

inline std::unordered_set<Keyword> requiredKeywords = {Keyword::Symbol,Keyword::Type,Keyword::Provider};