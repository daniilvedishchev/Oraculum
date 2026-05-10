#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "filemanager/file/file.hpp"

class FileManager {
    private:

        const std::filesystem::path _getUserName();
        const std::filesystem::path _getHomeDir();

        std::filesystem::path _path;
    public:
        FileManager();

        file createFile(const std::string& symbol, const std::string& type, bool overwrite = false);
        file createFile(const std::string& name, bool overwrite = false);

        std::filesystem::path envPath();

        bool dirExistsInLocalOraculumEnv(const std::string& directory);
};
