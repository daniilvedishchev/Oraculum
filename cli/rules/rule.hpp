#pragma once
#include <cstddef>
#include <functional>
#include "namespace/namespace.hpp"
#include "config/config.hpp"

struct Rule {
    oraculum::RuleValue mandatoryValue;
    oraculum::RuleValue defaultValue;
    std::function<bool(Config& cfg, const char* args[], int argc, size_t idx)> validate;
};
