#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>


class writer {
    private:
        void _createFile(const std::string& symbol, const std::string& type);

        const std::filesystem::path _getUserName();
        const std::filesystem::path _getHomeDir();

        std::filesystem::path _path;
        std::ofstream _file;
    public:
        writer();

};
