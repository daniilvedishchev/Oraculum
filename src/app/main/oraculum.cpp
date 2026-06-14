#include <iostream>
#include "src/app/main/oraculum.hpp"
#include "cacheservice/meta/metadata.hpp"
#include "config/config.hpp"
#include "orderbook/depth/depthUpdate.hpp"

namespace oraculum {
    oraculum::oraculum(int argc, char* argv[]):
    cli_(CLI(argc,argv)),cfg_(cli_.parseCliArgs()),
    fm_(FileManager()),cache_(CacheService(fm_)),validator_(Validator(cfg_,cache_)){
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
        if (cfg_.writeLiveData && (cfg_.type == "depth")){
            writeOrderBook();
        }
    }

    void oraculum::writeOrderBook(){
        try {
            OrderBookConstructor orderBook(cfg_, fm_, cache_);
            orderBook.start();
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            orderBook.stop();
        } catch(const std::exception& e){
            std::cerr<<"Error"<<e.what()<<std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        oraculum::oraculum oracul(argc,argv);
        oracul.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error running oraculum: " << e.what() << '\n';
        return 1;
    }
}
