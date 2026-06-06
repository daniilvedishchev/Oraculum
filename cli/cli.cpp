#include "cli/cli.hpp"

#include <stdexcept>

namespace oraculum {
    CLI::CLI(int argc, char* argv[]): cliArgs_(argv), cliSize_(argc){}

    Config CLI::parseCliArgs() const {
        Config cfg;

        for (int i = 1; i < cliSize_; ++i) {
            const std::string argument = cliArgs_[i];
            const auto keywordIt = kKeywordLookup.find(argument);
            
            if (keywordIt == kKeywordLookup.end()) {
                throw std::runtime_error("Unknown argument: " + argument);
            }

            switch (keywordIt->second) {
                case Keyword::WriteLiveData:
                    cfg.writeLiveData = true;
                    break;
                case Keyword::Provider:
                    if (i + 1 >= cliSize_) {
                        throw std::runtime_error("Missing value after '-provider'.");
                    }
                    cfg.provider = cliArgs_[++i];
                    break;
                case Keyword::Symbol:
                    if (i + 1 >= cliSize_) {
                        throw std::runtime_error("Missing value after '-symbol'.");
                    }
                    cfg.symbol = cliArgs_[++i];
                    break;
                case Keyword::Type:
                    if (i + 1 >= cliSize_) {
                        throw std::runtime_error("Missing value after '-type'.");
                    }
                    cfg.type = cliArgs_[++i];
                    break;
                case Keyword::Speed:
                    if (i + 1 >= cliSize_) {
                        throw std::runtime_error("Missing value after '-s'.");
                    }
                    cfg.speed = cliArgs_[++i];
                    break;
                case Keyword::TimeFrame:
                    if (i + 1 >= cliSize_) {
                        throw std::runtime_error("Missing value after '-tf'.");
                    }
                    cfg.tf = cliArgs_[++i];
                    break;
                case Keyword::Depth:
                    if (i + 1 >= cliSize_) {
                        throw std::runtime_error("Missing value after '-d'.");
                    }
                    cfg.depth = cliArgs_[++i];
                    break;
            }
        }

        return cfg;
    }
}
