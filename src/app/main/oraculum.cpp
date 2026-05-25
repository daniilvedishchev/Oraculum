#include <iostream>
#include "src/app/main/oraculum.hpp"

namespace oraculum {
    oraculum::oraculum(int argc, char* argv[]):
    cli_(CLI(argc,argv)),cfg_(cli_.parseCliArgs()),
    fm_(FileManager()),cache_(CacheService(fm_)),validator_(Validator(cfg_,cache_)){}

    void oraculum::run(){
        validator_.validate();
        if (cfg_.writeLiveData && (cfg_.type == "depth")){
            writeOrderBook();
        }
    }

    void oraculum::writeOrderBook(){
        std::string endpoint = makeOrderBookUpdateEndpoint(cfg_);
        std::string url = buildUrl(resolveProviderOrThrow(cfg_.provider),Connection::WebSocket,endpoint);
        try {
            OraculumSocket socket = OraculumSocket(cfg_.provider,url);
            OrderBookConstructor orderBook(cfg_, fm_, socket);
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
