#include "src/app/main/oraculum.hpp"
namespace oraculum {
    oraculum::oraculum(int argc, char* argv[]):
    cli_(std::move(CLI(argc,argv))),cfg_(std::move(cli_.parseCliArgs())),connector_(std::move(Connector{})),
    fm_(std::move(FileManager())),cache_(CacheService(fm_,connector_)),validator_(std::move(Validator(cfg_,cache_))){}

    void oraculum::run(){
        validator_.validate();
        if (cfg_.writeLiveData && (cfg_.type == "depth")){
            Provider provider = kStringToProvider.find(cfg_.provider)->second;
            makeOrderBookSnapshot(cfg_,connector_,provider,cfg_.symbol,fm_);
        }
    }
}


int main(int argc, char* argv[]) {
    oraculum::oraculum oracul(argc,argv);
    oracul.run();

}