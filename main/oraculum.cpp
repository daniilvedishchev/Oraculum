#include <iostream>
#include <string>
#include "writer/writer.hpp"

int main(int argc,char* argv[]) {
    if (argc > 1 && std::string(argv[1])=="--write-live-data"){
        writer W = writer();
        std::string symbol = argv[2];
        std::string type = argv[3];
        W._createFile(symbol,type);
    }
    return 0;
}