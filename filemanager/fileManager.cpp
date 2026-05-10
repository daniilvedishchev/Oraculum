#include "filemanager/FileManager.hpp"

FileManager::FileManager(){
    _path = _getHomeDir() / "Oraculum";
    if (!dirExistsInLocalOraculumEnv("cache")){
        std::filesystem::create_directories(_path/"cache");
    }
}

std::filesystem::path FileManager::envPath(){
    return _path;
};

bool FileManager::dirExistsInLocalOraculumEnv(const std::string& directory){
    return std::filesystem::exists(_path/directory);
}

file FileManager::createFile(const std::string& name, bool overwrite){
    std::ios::openmode method;
    if (!overwrite) {
        method = std::ios::app;
    } else method = std::ios::out;

    std::filesystem::path path = _path/name;

    std::ofstream fileobj = std::ofstream(path, method);

    if (!fileobj) {
        throw std::runtime_error("Cannot open file: " + path.string());
    }

    file f = file{.file=std::move(fileobj), .path=path};

    return f;
}

file FileManager::createFile(const std::string& symbol, const std::string& type, bool overwrite){
    std::ios::openmode method;
    if (!overwrite) {
        method = std::ios::app;
    } else method = std::ios::out;

    std::filesystem::path path = _path/(symbol+"-"+type);
    std::ofstream fileobj = std::ofstream(path, method);

    if (!fileobj) {
        throw std::runtime_error("Cannot open file: " + path.string());
    }

    file f = file{.file=std::move(fileobj), .path=path};

    return f;
}

const std::filesystem::path FileManager::_getUserName(){
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

const std::filesystem::path FileManager::_getHomeDir(){
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
