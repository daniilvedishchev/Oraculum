#pragma once

#include <filesystem>
#include <fstream>
#include <string>

namespace oraculum {

struct FileHandle {
    std::ofstream stream;
    std::filesystem::path path;
    void writeLine(const std::string& text);
};

} // namespace oraculum
