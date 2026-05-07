cmake_minimum_required(VERSION 3.20)

project(Oraculum)

set(CMAKE_CXX_STANDARD 17)

add_executable(ORACULUM main/oraculum.cpp)

target_include_directories(ORACULUM PRIVATE /usr/local/include)
target_link_libraries(ORACULUM PRIVATE /usr/local/lib/libixwebsocket.a)
