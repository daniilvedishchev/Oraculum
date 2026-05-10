cmake_minimum_required(VERSION 3.20)

project(Oraculum)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/cpr)
add_subdirectory(external/json)

add_executable(ORACULUM
    src/app/oraculum.cpp
    filemanager/FileManager.cpp
    cacheservice/CacheService.cpp
    filemanager/file/file.cpp
)

target_include_directories(ORACULUM PRIVATE 
    ${CMAKE_SOURCE_DIR}
    /usr/local/include
)

target_link_libraries(ORACULUM PRIVATE
    /usr/local/lib/libixwebsocket.a
    cpr::cpr
    nlohmann_json::nlohmann_json
)