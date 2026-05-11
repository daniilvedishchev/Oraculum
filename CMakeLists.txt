cmake_minimum_required(VERSION 3.20)

project(Oraculum)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/cpr)
add_subdirectory(external/json)
add_subdirectory(external/IXWebSocket)

add_executable(ORACULUM
    src/app/oraculum.cpp
    filemanager/fileManager.cpp
    cacheservice/CacheService.cpp
    filemanager/file/file.cpp
    cli/cli.cpp
)

target_include_directories(ORACULUM PRIVATE 
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(ORACULUM PRIVATE
    ixwebsocket::ixwebsocket
    cpr::cpr
    nlohmann_json::nlohmann_json
)
