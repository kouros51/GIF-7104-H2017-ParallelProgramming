cmake_minimum_required(VERSION 3.6)
project(SourceFiles)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Chrono.hpp
        premiers.cpp)

add_executable(Primes ${SOURCE_FILES})
target_link_libraries(Primes pthread)