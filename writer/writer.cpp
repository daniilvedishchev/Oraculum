#pragma once
#include "writer.hpp"

const std::string writer::_getUserName(){
    #ifdef _WIN32
        const std::string username = std::getenv("USERNAME");
    #endif
}
