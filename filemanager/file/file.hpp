#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

struct file {
    std::ofstream file;
    std::filesystem::path path;
    void write(const std::string& txt);
};