cmake_minimum_required(VERSION 3.16)
project(mandarinfish)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
else ()
    add_definitions(-DDEBUG=0)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(LIB_PREFIX "/usr/local/lib")

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

find_package(Boost QUIET REQUIRED COMPONENTS program_options)

add_executable(mandarinfish main.cc
        src/streaming.cc
        src/streaming.h
        src/sole.h src/ThreadPool.h)

target_link_libraries(mandarinfish
        pthread
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::program_options
        cppkafka
        rdkafka
        elasticlient
        cpr
        jsoncpp
        )