#pragma once
#include <filesystem>
#include <string>

#include "filemanager/file/file.hpp"

namespace oraculum {

class FileManager {
private:
    std::filesystem::path getHomeDir();

    std::filesystem::path path_;

public:
    FileManager();

    FileHandle createFile(const std::string& symbol, const std::string& type, bool overwrite = false);
    FileHandle createFile(const std::string& name, bool overwrite = false);

    std::filesystem::path environmentPath() const;

    bool directoryExistsInOraculumEnv(const std::string& directory) const;
};

} // namespace oraculum
