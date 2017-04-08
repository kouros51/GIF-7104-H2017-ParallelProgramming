cmake_minimum_required(VERSION 3.6)
project(SourceCode)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenCL REQUIRED)
include_directories( ${OPENCL_INCLUDE_DIR} )
link_directories(${OpenCL_LIBRARY})

set(SOURCE_FILES
        Headers/Chrono.hpp
        Headers/lodepng.h
        Headers/tp4_openCL.h
        PACC/Assert.hpp
        PACC/config.hpp
        PACC/Tokenizer.hpp
        PACC/Tokenizer.cpp
        lodepng.cpp
        tp4.cpp
        tp4_openCL.cpp)

add_executable(tp4 ${SOURCE_FILES})
target_link_libraries( tp4 ${OpenCL_LIBRARY} )