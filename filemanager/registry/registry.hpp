#pragma once

#include <unordered_map>
#include <iostream>

#include "config/config.hpp"
#include "filemanager/file/file.hpp"
#include "filemanager/fileManager.hpp"
#include "trades/structure/trades.h"
#include "orderbook/features/structure/featureStructure.hpp"

struct oraculumOutputs{
    oraculum::FileHandle liquidations;
    oraculum::FileHandle trades;
    oraculum::FileHandle snapshots;
    oraculum::FileHandle updates;
    oraculum::FileHandle features;
};

namespace oraculum {
    class FileRegistry {
        private:
            FileManager& manager_;
            Config& cfg_;
            void scanConfigOptionsCreateFiles();
        public:
            FileRegistry(Config& ,FileManager&);
            oraculumOutputs getAllFileObjects();
            oraculumOutputs files;
    };
}
