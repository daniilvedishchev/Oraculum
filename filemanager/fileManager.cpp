#include "filemanager/fileManager.hpp"

#include <cstdlib>

namespace oraculum {

FileManager::FileManager() {
    path_ = getHomeDir() / "Oraculum";
    if (!directoryExistsInOraculumEnv("cache")) {
        std::filesystem::create_directories(path_ / "cache");
    }
}

std::filesystem::path FileManager::environmentPath() const {
    return path_;
}

bool FileManager::directoryExistsInOraculumEnv(const std::string& directory) const {
    return std::filesystem::exists(path_ / directory);
}

FileHandle FileManager::createFile(const std::string& name, bool overwrite) const {
    const std::ios::openmode mode = overwrite ? std::ios::out : std::ios::app;
    const std::filesystem::path path = path_ / name;

    std::ofstream stream(path, mode);
    if (!stream) {
        throw std::runtime_error("Cannot open file: " + path.string());
    }

    return FileHandle{std::move(stream), path};
}

FileHandle FileManager::createFile(const std::string& symbol, const std::string& type, bool overwrite) const {
    return createFile(symbol + "-" + type, overwrite);
}

std::filesystem::path FileManager::getHomeDir() {
    #ifdef _WIN32
        const char* homeDir = std::getenv("USERPROFILE");
    #else
        const char* homeDir = std::getenv("HOME");
    #endif

    if (!homeDir) {
        throw std::runtime_error("No valid homeDir found.");
    }

    return std::filesystem::path(homeDir);
}

} // namespace oraculum
