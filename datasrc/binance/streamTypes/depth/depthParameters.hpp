#pragma once

#include <unordered_set>
namespace oraculum {
    struct depth {
        std::unordered_set<int16_t> value = {5,10,20,50,100,500,1000};
    };
}