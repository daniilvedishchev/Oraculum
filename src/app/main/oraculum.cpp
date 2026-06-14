#include <iostream>
#include <thread>
#include "src/app/main/oraculum.hpp"
#include "cacheservice/meta/metadata.hpp"
#include "config/config.hpp"
#include "datasrc/resolvers/providerResolver.hpp"
#include "orderbook/constructor/orderBookConstructor.hpp"
#include "orderbook/depth/depthUpdate.hpp"
#include "trades/aggregated/aggregateTradesStream.hpp"
#include "trades/liquidations/liquidationsStream.hpp"

namespace oraculum {

    void handleSignal(int signal) {
        if (signal == SIGINT) {
            g_running = false;
        }
    }

    oraculum::oraculum(int argc, char* argv[]):
    cli_(CLI(argc,argv)),cfg_(cli_.parseCliArgs()),
    fm_(FileManager()),cache_(CacheService(fm_)),validator_(Validator(cfg_,cache_)){
        cache_.loadOrUpdateSymbols(resolveProviderOrThrow(cfg_.provider));
        setConfigMetadata();
        registry_.emplace(cfg_,fm_);
    }

    void oraculum::setConfigMetadata(){
        MetaData meta = cache_.getMetaBySymbolOrThrow(cfg_.symbol);
        cfg_.tickSize = meta.tickSize;
        cfg_.stepSize = meta.stepSize;
    }

    void oraculum::run(){
        validator_.validate();

        OrderBookConstructor orderbook(cfg_, fm_, cache_,registry_.value());
        AggregateTradeStream trades(cfg_,registry_.value());
        LiquidationsStream liquidations(cfg_,registry_.value());


        if (cfg_.snapshots && cfg_.updates) orderbook.start();
        if (cfg_.aggTrades) trades.start();
        if (cfg_.liquidations) liquidations.start();
        
        while (g_running){
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (cfg_.snapshots && cfg_.updates && (cfg_.type == "depth")) orderbook.stop();
        if (cfg_.aggTrades) trades.stop();
        if (cfg_.liquidations) liquidations.stop();
    }
}

int main(int argc, char* argv[]) {
    try {
        std::signal(SIGINT, oraculum::handleSignal);
        oraculum::oraculum oracul(argc,argv);
        oracul.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error running oraculum: " << e.what() << '\n';
        return 1;
    }
}
