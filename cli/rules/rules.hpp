#pragma once

#include <functional>

#include "namespace/namespace.hpp"
#include "config/config.hpp"
struct rule {
    Oraculum::value mandatory;
    Oraculum::value valuebydefault;
    std::function<bool(config& cfg, const char* args[], const int argc, size_t idx)> validate;
};