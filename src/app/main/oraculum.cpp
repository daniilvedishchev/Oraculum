#include "src/app/main/oraculum.hpp"
namespace oraculum {
    oraculum::oraculum(int argc, char* argv[]):
    cli_(std::move(CLI(argc,argv))),cfg_(std::move(cli_.parseCliArgs())),
    fm_(std::move(FileManager())),cache_(CacheService(fm_)),validator_(std::move(Validator(cfg_,cache_))){}

    void oraculum::run(){
        validator_.validate();
        // if (cfg_.writeLiveData){
        //     makeSnapshot()
        //     writelive()
        // }
    }
}


int main(int argc, char* argv[]) {
    oraculum::oraculum oracul(argc,argv);
    oracul.run();

}