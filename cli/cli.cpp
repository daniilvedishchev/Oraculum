#include "cli/cli.hpp"
#include "keywords/keywords.hpp"

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
                case Keyword::OrderBook:
                    cfg.orderbook = true;
                    cfg.snapshots = true;
                    cfg.updates = true;
                    cfg.type = "depth";
                    break;
                case Keyword::AggregatedTrades:
                    cfg.aggTrades = true;
                    break;
                case Keyword::Liquidations:
                    cfg.liquidations = true;
                    break;
                case Keyword::Features:
                    if (!cfg.orderbook){
                        throw std::runtime_error("Impossible to calculate features without orderbook data, \
                        please check if '-orderbook' is placed before '-features' keyword.");
                    }
                    cfg.features = true;
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
