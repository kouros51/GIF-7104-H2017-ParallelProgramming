CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(tp5)

# Find CUDA and OpenCL
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
FIND_PACKAGE(CUDA QUIET)
FIND_PACKAGE(OpenCL QUIET)

# If the project is not being built at the same time as ArrayFire,
# we need to first find the ArrayFire library
IF(TARGET afcpu OR TARGET afcuda OR TARGET afopencl OR TARGET af)
    SET(ArrayFire_CPU_FOUND False)
    SET(ArrayFire_CUDA_FOUND False)
    SET(ArrayFire_OpenCL_FOUND False)
    SET(ArrayFire_Unified_FOUND False)
    SET(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../assets")
    IF(NOT EXISTS "${ASSETS_DIR}/LICENSE")
        MESSAGE(STATUS "Assests submodule unavailable. Updating submodules.")
        EXECUTE_PROCESS(
                COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                OUTPUT_QUIET
        )
    ENDIF()
ELSE()
    SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
    FIND_PACKAGE(ArrayFire REQUIRED)
    INCLUDE_DIRECTORIES(${ArrayFire_INCLUDE_DIRS})

    SET(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
ENDIF()

# A macro to build an ArrayFire example
# For most uses only FIND_PACKAGE(ArrayFire REQUIRED), ADD_EXECUTABLE(...)
# and TARGET_LINK_LIBRARIES(... ${ARRAYFIRE_LIBRARIES}) are needed
MACRO(BUILD_SIMULATION SIMULATION_NAME SIMULATION_SOURCE BACKEND_NAME BACKEND_LIBRARIES OTHER_LIBRARIES SIMULATION_LIBRARIES OUT_DIR_NAME)
    ADD_EXECUTABLE(${SIMULATION_NAME}_${BACKEND_NAME} ${SIMULATION_SOURCE})
    TARGET_LINK_LIBRARIES(${SIMULATION_NAME}_${BACKEND_NAME}
            ${BACKEND_LIBRARIES} ${OTHER_LIBRARIES} ${SIMULATION_LIBRARIES})
    SET_TARGET_PROPERTIES(${SIMULATION_NAME}_${BACKEND_NAME}
            PROPERTIES
            OUTPUT_NAME ${SIMULATION_NAME}_${BACKEND_NAME}
            RUNTIME_OUTPUT_DIRECTORY "executables"
            FOLDER "tp5/${BACKEND_NAME}")
ENDMACRO()

# Collect the source
FILE(GLOB FILES "*.cpp")
GET_FILENAME_COMPONENT(EXECUTABLE_NAME ${FILES} NAME_WE)
FILE(GLOB SIMULATION_LIBRARIES "src/thermalSimulation.cpp")
LIST(SORT FILES)
ADD_DEFINITIONS("-DASSETS_DIR=\"${ASSETS_DIR}\"")

# Next we build each example using CPU backend.
IF(${ArrayFire_CPU_FOUND})  # variable defined by FIND(ArrayFire ...)
    OPTION(BUILD_CPU "Build ArrayFire Examples for CPU backend" ON)
    BUILD_SIMULATION("${EXECUTABLE_NAME}" ${FILES} cpu_sequential ${ArrayFire_CPU_LIBRARIES} "" ${SIMULATION_LIBRARIES} "")
ELSEIF(TARGET afcpu)        # variable defined by the ArrayFire build tree
    BUILD_SIMULATION("${EXECUTABLE_NAME}" ${FILES} cpu afcpu "" ${SIMULATION_LIBRARIES} "")
ELSE()
    MESSAGE(STATUS "WARNING: CPU backend is OFF. afcpu was not found.")
ENDIF()


# Next we build each example using UNIFIED backend.
IF(${ArrayFire_Unified_FOUND})  # variable defined by FIND(ArrayFire ...)
    OPTION(BUILD_UNIFIED "Build ArrayFire Examples for Unified backend" ON)
    BUILD_SIMULATION("${EXECUTABLE_NAME}" ${FILES} unified ${ArrayFire_Unified_LIBRARIES} "${CMAKE_DL_LIBS}" ${SIMULATION_LIBRARIES} "")
ELSEIF(TARGET af)        # variable defined by the ArrayFire build tree
    BUILD_SIMULATIO("${EXECUTABLE_NAME}" ${FILES} unified af "${CMAKE_DL_LIBS}" ${SIMULATION_LIBRARIES} "")
ELSE()
    MESSAGE(STATUS "EXAMPLES: UNIFIED backend is OFF. af was not found.")
ENDIF()

# Next we build each example using OpenCL backend.
IF (${OpenCL_FOUND})
    IF(${ArrayFire_OpenCL_FOUND})  # variable defined by FIND(ArrayFire ...)
        OPTION(BUILD_OPENCL "Build ArrayFire Examples for OpenCL backend" ON)
        BUILD_SIMULATION("${EXECUTABLE_NAME}" ${FILES} opencl ${ArrayFire_OpenCL_LIBRARIES} "${OpenCL_LIBRARIES}" ${SIMULATION_LIBRARIES} "")
    ELSEIF(TARGET afopencl)        # variable defined by the ArrayFire build tree
        BUILD_SIMULATION("${EXECUTABLE_NAME}" ${FILES} opencl afopencl "${OpenCL_LIBRARIES}" ${SIMULATION_LIBRARIES} "")
    ELSE()
        MESSAGE(STATUS "EXAMPLES: OpenCL backend is OFF. afopencl was not found")
    ENDIF()
ELSE()
    MESSAGE(STATUS "EXAMPLES: OpenCL backend is OFF. OpenCL was not found")
ENDIF()


# Next we build each example using CUDA backend.
IF (${CUDA_FOUND})
    IF(${ArrayFire_CUDA_FOUND})  # variable defined by FIND(ArrayFire ...)
        # Find NVVM
        FIND_LIBRARY(CUDA_nvvm_LIBRARY
                NAMES "nvvm"
                PATH_SUFFIXES "nvvm/lib64" "nvvm/lib" "nvvm/lib/x64"
                PATHS ${CUDA_TOOLKIT_ROOT_DIR}
                DOC "CUDA NVVM Library"
                )
        MARK_AS_ADVANCED(CUDA_nvvm_LIBRARY)

        # If CUDA_CUDA_LIBRARY is not found, check for Stub in CUDA Toolkit
        IF(NOT CUDA_CUDA_LIBRARY)
            MESSAGE(SEND_ERROR "CMake CUDA Variable CUDA_CUDA_LIBRARY Not found.")
            MESSAGE("CUDA Driver Library (libcuda.so/libcuda.dylib/cuda.lib) cannot be found.")
            FIND_FILE(CUDA_CUDA_LIBRARY_STUB
                    NAMES "libcuda.so" "libcuda.dylib" "cuda.lib"
                    PATHS ${CUDA_TOOLKIT_ROOT_DIR}
                    PATH_SUFFIXES "lib64" "lib64/stubs" "lib" "lib/stubs" "lib/x64" "lib/Win32"
                    DOC "CUDA Library STUB"
                    )
            IF(CUDA_CUDA_LIBRARY_STUB)
                MESSAGE("You can use the library stub available in the CUDA Toolkit: ${CUDA_CUDA_LIBRARY_STUB}")
                MESSAGE("Run the following commands (Linux) to set it up:")
                MESSAGE("ln -s ${CUDA_CUDA_LIBRARY_STUB} /usr/lib/libcuda.so.1")
                MESSAGE("ln -s /usr/lib/libcuda.so.1 /usr/lib/libcuda.so")
            ENDIF()
            MESSAGE(FATAL_ERROR "Ending CMake configuration because of missing CUDA_CUDA_LIBRARY")
        ENDIF(NOT CUDA_CUDA_LIBRARY)

        OPTION(BUILD_CUDA "Build ArrayFire Examples for CUDA backend" ON)
        BUILD_SIMULATION("${EXECUTABLE_NAME}" ${FILES} cuda ${ArrayFire_CUDA_LIBRARIES} "${CUDA_CUBLAS_LIBRARIES};${CUDA_LIBRARIES};${CUDA_cusolver_LIBRARY};${CUDA_cusparse_LIBRARY};${CUDA_CUFFT_LIBRARIES};${CUDA_nvvm_LIBRARY};${CUDA_CUDA_LIBRARY}" ${SIMULATION_LIBRARIES} "")
    ELSEIF(TARGET afcuda)        # variable defined by the ArrayFire build tree
        BUILD_SIMULATION("${EXECUTABLE_NAME}" ${FILES} cuda afcuda "" ${SIMULATION_LIBRARIES} "")
    ELSE()
        MESSAGE(STATUS "EXAMPLES: CUDA backend is OFF. afcuda was not found")
    ENDIF()
ELSE()
    MESSAGE(STATUS "EXAMPLES: CUDA backend is OFF. CUDA was not found")
ENDIF()