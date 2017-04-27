cmake_minimum_required(VERSION 3.6)
project(SourceCode)

set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(ArrayFire REQUIRED)
INCLUDE_DIRECTORIES(${ArrayFire_INCLUDE_DIRS})

find_package(OpenCL REQUIRED)
include_directories( ${OPENCL_INCLUDE_DIR} )
link_directories(${OpenCL_LIBRARY})

ADD_EXECUTABLE(tp5 arrayFireTestWokingInstallation.cpp)
TARGET_LINK_LIBRARIES(tp5 ${OpenCL_LIBRARY})
TARGET_LINK_LIBRARIES(tp5 ${ArrayFire_LIBRARIES})
