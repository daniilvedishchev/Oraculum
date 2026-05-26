cmake_minimum_required(VERSION 3.20)

project(Oraculum)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/cpr)
add_subdirectory(external/json)

set(USE_TLS ON CACHE BOOL "Enable TLS support" FORCE)
set(USE_OPEN_SSL ON CACHE BOOL "Use OpenSSL for TLS" FORCE)

if(APPLE)
    message(STATUS "Building on macOS")

    # Apple Silicon: /opt/homebrew
    if(EXISTS "/opt/homebrew/opt/openssl@3")
        set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3" CACHE PATH "OpenSSL root" FORCE)

    # Intel Mac: /usr/local
    elseif(EXISTS "/usr/local/opt/openssl@3")
        set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl@3" CACHE PATH "OpenSSL root" FORCE)
    endif()

elseif(WIN32)
    message(STATUS "Building on Windows")

elseif(UNIX)
    message(STATUS "Building on Linux/Unix")
endif()

add_subdirectory(external/IXWebSocket)

add_executable(ORACULUM
    src/app/main/oraculum.cpp
    src/app/validation/validator.cpp
    orderbook/constructor/orderBookConstructor.cpp
    filemanager/fileManager.cpp
    cacheservice/CacheService.cpp
    datasrc/providers/providers.cpp
    datasrc/resolvers/providerResolver.cpp
    filemanager/file/file.cpp
    cli/cli.cpp
    socket/oraculumSocket/oraculumSocket.cpp
    socket/callbacks/callback.cpp
)

target_include_directories(ORACULUM PRIVATE 
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(ORACULUM PRIVATE
    ixwebsocket::ixwebsocket
    cpr::cpr
    nlohmann_json::nlohmann_json
)
