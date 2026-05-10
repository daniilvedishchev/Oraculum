#include "filemanager/file/file.hpp"

namespace oraculum {

void FileHandle::writeLine(const std::string& text) {
    stream << text << "\n";
}

} // namespace oraculum
