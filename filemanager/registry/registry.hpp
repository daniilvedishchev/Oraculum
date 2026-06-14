#pragma once

#include "config/config.hpp"
#include "filemanager/file/file.hpp"
#include "filemanager/fileManager.hpp"
#include <unordered_map>

struct oraculumOutputs{
    oraculum::FileHandle liquidations;
    oraculum::FileHandle trades;
    oraculum::FileHandle snapshots;
    oraculum::FileHandle updates;
    oraculum::FileHandle features;
};

namespace oraculum {
    class FileRegistry {
        FileManager& manager_;
        Config& cfg_;
        oraculumOutputs files_;
        void scanConfigOptionsCreateFiles();
        public:
            FileRegistry(Config& ,FileManager&);
    };
}
