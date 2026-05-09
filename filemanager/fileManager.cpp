#include "filemanager/fileManager.hpp"

fileManager::fileManager(){
    _path = _getHomeDir() / "Oraculum";
    if (!dirExistsInLocalOraculumEnv("cache")){
        std::filesystem::create_directories(_path/"cache");
    }
}

bool fileManager::dirExistsInLocalOraculumEnv(const std::string& directory){
    return std::filesystem::exists(_path/directory);
}

void fileManager::_createFile(const std::string& symbol,const std::string& type){
    std::cout<< std::string(_path/(symbol+"-"+type))<<"\n";
    _file = std::ofstream(_path/(symbol+"-"+type), std::ios::app);

    if (!_file) {
        throw std::runtime_error("Cannot open file: " + _path.string());
    }
}

const std::filesystem::path fileManager::_getUserName(){
    #ifdef _WIN32
        const char* user = std::getenv("USERNAME");
    #else
        const char* user = std::getenv("USER");
    #endif

    if (!user){
        throw std::runtime_error("No valid user name found.");
        return "";
    }

    return std::filesystem::path(user);
}

const std::filesystem::path fileManager::_getHomeDir(){
    #ifdef _WIN32
        const char* homeDir = std::getenv("USERPROFILE");
    #else
        const char* homeDir = std::getenv("HOME");
    #endif

    if (!homeDir){
        throw std::runtime_error("No valid homeDir found.");
        return "";
    }

    return std::filesystem::path(homeDir);
}
