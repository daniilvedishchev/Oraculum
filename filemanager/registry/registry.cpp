#include "filemanager/registry/registry.hpp"
#include <filesystem>
#include <unordered_map>

namespace oraculum {
    FileRegistry::FileRegistry(Config& cfg,FileManager& fm):cfg_(cfg),manager_(fm){}

    void FileRegistry::scanConfigOptionsCreateFiles(){

        struct OutputSpec {
            bool enabled;
            const char* directory;
            const char* filename;
            FileHandle oraculumOutputs::* target;
        };

        OutputSpec specs[] = {
            {cfg_.aggTrades, "aggregatedTrades", "trades.csv", &oraculumOutputs::trades},
            {cfg_.liquidations, "liquidations", "liquidations.csv", &oraculumOutputs::liquidations},
            {cfg_.features,"features","features.csv",&oraculumOutputs::features},
            {cfg_.snapshots,"orderbook/snapshots","snapshots.csv",&oraculumOutputs::snapshots},
            {cfg_.updates,"orderbook/updates","updates.csv",&oraculumOutputs::updates}
        };

        for (const auto& spec : specs) {
            if (!spec.enabled) continue;
            
            auto path = manager_.environmentPath() / cfg_.symbol / spec.directory;
            files_.*(spec.target) = manager_.createFile(spec.filename, path, false);
        }
    }
}