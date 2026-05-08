#include <iostream>
#include <string>
#include "writer/writer.hpp"

int main(int argc,char* argv[]) {
    if (argc>1 && std::string(argv[1])=="--write-live-data"){
        std::cout << "Hello ORACULUM" << std::endl;
    }
    writer W = writer();
    std::string symbol = "btcusd";
    std::string type = "depth";
    W._createFile(symbol,type);
    return 0;
}