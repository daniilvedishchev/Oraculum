#include "src/app/main/oraculum.hpp"

#include <iostream>



namespace oraculum {
    oraculum::oraculum(int argc, char* argv[]):
    cli_(CLI(argc,argv)),cfg_(cli_.parseCliArgs()),
    fm_(FileManager()),cache_(CacheService(fm_)),validator_(Validator(cfg_,cache_)){}

    void oraculum::run(){
        validator_.validate();
        if (cfg_.writeLiveData && (cfg_.type == "depth")){
            auto it = kStringToProvider.find(cfg_.provider);
            if (it == kStringToProvider.end()){
                throw std::runtime_error("Invalid provider.");
            }
            Provider provider = it->second;
            makeOrderBookSnapshot(cfg_,provider,cfg_.symbol,fm_);
            // writeOrderBookUpdates()
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
