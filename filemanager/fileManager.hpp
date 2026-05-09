#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>


class fileManager {
    private:

        const std::filesystem::path _getUserName();
        const std::filesystem::path _getHomeDir();

        std::filesystem::path _path;
        std::ofstream _file;
    public:
        fileManager();

        void _createFile(const std::string& symbol, const std::string& type);
        bool dirExistsInLocalOraculumEnv(const std::string& directory);
        

};
