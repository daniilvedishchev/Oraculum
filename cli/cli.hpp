#pragma once

#include "config/config.hpp"
#include "keywords/keywords.hpp"

class CLI {
private:
    int cliSize_;
    char** cliArgs_;
public:
    CLI(int argc, char* argv[]);
    Config parseCliArgs() const;
};
