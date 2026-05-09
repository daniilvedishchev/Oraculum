#include <iostream>
#include <string>

#include "filemanager/fileManager.cpp"

int main(int argc,char* argv[]) {
    fileManager W = fileManager();
    if (argc > 1 && std::string(argv[1])=="--write-live-data"){
        fileManager W = fileManager();
        std::string symbol = argv[2];
        std::string type = argv[3];
        W._createFile(symbol,type);
    }
    return 0;
}