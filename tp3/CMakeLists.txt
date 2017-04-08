cmake_minimum_required (VERSION 2.6)

project (GaussJordanMPI)

#### MPI
find_package(MPI REQUIRED)

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)

add_executable(tp3 main.cpp Matrix.cpp)


INCLUDE(CPack)
