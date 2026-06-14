#include "filemanager/registry/registry.hpp"
#include "orderbook/features/structure/header.hpp"
#include <filesystem>
#include <unordered_map>



namespace oraculum {
    struct OutputSpec {
        bool enabled;
        const std::string directory;
        const std::string filename;
        const std::string extension;
        FileHandle oraculumOutputs::* target;
    };

    std::unordered_map<std::string, std::string> marketEventToFileHeader = {
       {"aggregatedTrades", aggregatedTradesHeader},
       {"liquidations", liquidationsHeader},
       {"features", featuresHeader}
    };
    
    FileRegistry::FileRegistry(Config& cfg,FileManager& fm):cfg_(cfg),manager_(fm){
        scanConfigOptionsCreateFiles();
    }

    void FileRegistry::scanConfigOptionsCreateFiles(){

        OutputSpec specs[] = {
            {cfg_.aggTrades,"aggregatedTrades", "trades","csv", &oraculumOutputs::trades},
            {cfg_.liquidations,"liquidations", "liquidations","csv", &oraculumOutputs::liquidations},
            {cfg_.features,"features","features", "csv", &oraculumOutputs::features},
            {cfg_.snapshots,"orderbook/snapshots","snapshots","json", &oraculumOutputs::snapshots},
            {cfg_.updates,"orderbook/updates","updates","json", &oraculumOutputs::updates}
        };

        for (const auto& spec : specs) {
            if (!spec.enabled) continue;

            auto path = manager_.environmentPath() / cfg_.symbol / spec.directory;
            const auto filepath = std::filesystem::path(path/(spec.filename+"."+spec.extension));

            bool shouldWriteHeader = !std::filesystem::exists(filepath) ||
                (std::filesystem::exists(filepath) && std::filesystem::file_size(filepath) == 0);

            files.*(spec.target) = manager_.createFile(spec.filename + "." + spec.extension, path, false);

            if (spec.extension == "csv") {
                auto itHeader = marketEventToFileHeader.find(spec.directory);
                if (itHeader != marketEventToFileHeader.end() && shouldWriteHeader){
                    (files.*(spec.target)).writeLine(itHeader->second);
                }
            }
        }
    }
}
