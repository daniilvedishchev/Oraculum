#include <iostream>
#include <string>

#include "filemanager/FileManager.hpp"

int main(int argc,char* argv[]) {
    FileManager W = FileManager();
    if (argc > 1 && std::string(argv[1])=="--write-live-data"){
        FileManager W = FileManager();
        std::string symbol = argv[2];
        std::string type = argv[3];
        W._createFile(symbol,type);
    }
    return 0;
}