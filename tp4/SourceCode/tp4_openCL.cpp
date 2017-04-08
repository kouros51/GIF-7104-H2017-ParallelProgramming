#include <stdio.h>
#include <CL/cl.h>
#include <CL/opencl.h>
#include <vector>
#include <fstream>
#include <iostream>

#include "Headers/tp4_openCL.h"

void tp4_openCL::initialize() {

    // Query for the number of recognized platforms
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (status != CL_SUCCESS) {
        printf("Error code:\t%d\n", status);
        if (status == CL_PLATFORM_NOT_FOUND_KHR) {
            printf("No valid ICDs found, check the installation of your OpenCL implementation.\n");
        } else {
            printf("Unknown clGetPlatformIDs error, check the error code in OpenCL documentation.\n");
        }
        exit(-1);
    }

    // Make sure some platforms were found
    if (numPlatforms == 0) {
        printf("No platforms detected.\n");
        exit(-1);
    }

    // Allocate enough space for each platform
    platforms = (cl_platform_id *) malloc(numPlatforms * sizeof(cl_platform_id));
    if (platforms == NULL) {
        perror("malloc");
        exit(-1);
    }

    // Fill in platforms
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    if (status != CL_SUCCESS) {
        printf("clGetPlatformIDs failed\n");
        exit(-1);
    }

    // Query for the number of devices present
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
    if (status != CL_SUCCESS) {
        printf("clGetDeviceIDs failed\n");
        exit(-1);
    }

    // Make sure some devices were found
    if (numDevices == 0) {
        printf("No devices detected.\n");
        exit(-1);
    }

    // Allocate enough space for each device
    devices = (cl_device_id *) malloc(numDevices * sizeof
            (cl_device_id));
    if (devices == NULL) {
        perror("malloc");
        exit(-1);
    }

    // Fill in devices
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
    if (status != CL_SUCCESS) {
        printf("Acquiring devices failed\n");
        exit(-1);
    }
}

void tp4_openCL::getPlatformsInfo() {

    // Print out some basic information about each platform
    printf("%u platforms detected\n", numPlatforms);
    for (unsigned int i = 0; i < numPlatforms; i++) {
        char buf[100];
        printf("Platform %u: \n", i);
        status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(buf), buf, NULL);
        printf("\tVendor: %s\n", buf);
        status |= clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(buf), buf, NULL);
        printf("\tName: %s\n", buf);
        status |= clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(buf), buf, NULL);
        printf("\tPlatform version: %s\n", buf);
        if (status != CL_SUCCESS) {
            printf("clGetPlatformInfo failed\n");
            exit(-1);
        }
    }
    printf("\n");
}

void tp4_openCL::getDevicesInfo() {

    // Print out some informations about each platform
    printf("%u devices found.\n", numDevices);
    for (unsigned int i = 0; i < numDevices; i++) {
        char buf[100];
        cl_device_type type;
        printf("Device number %i:\n", i);

        // Getting device Type
        status = clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(type), &type, NULL);
        switch (type) {
            case CL_DEVICE_TYPE_DEFAULT:
                printf("\tCL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_DEFAULT");
                break;
            case CL_DEVICE_TYPE_CPU:
                printf("\tCL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_CPU");
                break;
            case CL_DEVICE_TYPE_GPU:
                printf("\tCL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_GPU");
                break;
            case CL_DEVICE_TYPE_ACCELERATOR:
                printf("\tCL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_ACCELERATOR");
                break;
            case CL_DEVICE_TYPE_CUSTOM:
                printf("\tCL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_CUSTOM");
                break;
            default:
                printf("\tCL_DEVICE_TYPE: %s\n", "UNKNOWN");
        }

        // Getting device Type
        status |= clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buf), &buf, NULL);
        printf("\tCL_DEVICE_VENDOR: %s\n", buf);

        // Getting device Type
        status |= clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buf), &buf, NULL);
        printf("\tCL_DEVICE_NAME: %s\n", buf);

        // Getting device Type
        cl_uint max_compute_units;
        status |= clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units),
                                  &max_compute_units, NULL);
        printf("\tCL_DEVICE_MAX_COMPUTE_UNITS: %u\n", max_compute_units);

        // Getting device Type
        size_t max_work_group_size;
        status |= clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size),
                                  &max_work_group_size, NULL);
        printf("\tCL_DEVICE_MAX_WORK_GROUP_SIZE: %u\n", max_work_group_size);

        if (status != CL_SUCCESS) {
            printf("clGetDeviceIDs failed\n");
            exit(-1);
        }

    }
}

void tp4_openCL::initializeContext() {
    context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
    if (status != CL_SUCCESS || context == NULL) {
        printf("clCreateContext failed.");
        exit(-1);
    }
}

void tp4_openCL::initializeCommandQueue() {
    this->cmdQueue = clCreateCommandQueueWithProperties(context, devices[0], 0, &status);
    if (status != CL_SUCCESS || cmdQueue == NULL) {
        printf("clCreateCommandQueueWithProperties failed.");
        exit(-1);
    }
}

void tp4_openCL::initializeBuffer(std::vector<unsigned char> gImage, unsigned int width, unsigned int height,
                                  int filterSize, double *filter) {
    printf("Allocating buffers for the kernel.\n");
    inImage = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(char) * width * height, &gImage, &status);
    inFilter = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(double) * filterSize*filterSize,filter,&status);
    inFilterSize = clCreateBuffer(context,CL_MEM_READ_ONLY,sizeof(int),&filterSize,&status);
    inWidth = clCreateBuffer(context,CL_MEM_READ_ONLY,sizeof(int),&width,&status);
    outImage = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(char) * width * height, &gImage, &status);
}

char *tp4_openCL::readOpenClKernelFile() {
    printf("Reading kernel from source.\n");
    FILE *sourceFile = fopen("filter.cl", "rb");
    fseek(sourceFile, 0, SEEK_END);
    size_t sourceCodeSize = ftell(sourceFile);
    rewind(sourceFile);

    char *programBuffer = (char *) malloc(sourceCodeSize + 1);
    programBuffer[sourceCodeSize] = '\0';
    fread(programBuffer, sizeof(char), sourceCodeSize, sourceFile);
    fclose(sourceFile);
    return programBuffer;
}

void tp4_openCL::initializeProgram(char *kernelSource) {
    printf("Initializing program from the kernel.\n");
    program = clCreateProgramWithSource(context, 1, (const char **) &kernelSource, NULL, &status);
    if (status != CL_SUCCESS) {
        printf("clCreateProgramWithSource failed.");
        exit(-1);
    }
}

void tp4_openCL::buildProgram() {
    printf("Building the program.\n");
    buildErr = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);

    if (buildErr != CL_SUCCESS) {
        printf("Program failed to build.\n");
        cl_build_status buildStatus;
        for (unsigned int i = 0; i < numDevices; i++) {
            clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &buildStatus,
                                  NULL);
            if (buildStatus == CL_SUCCESS) {
                continue;
            }

            char *buildLog;
            size_t buildLogSize;
            clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_LOG, 0, NULL, &buildLogSize);
            buildLog = (char *) malloc(buildLogSize);
            if (buildLog == NULL) {
                perror("malloc");
                exit(-1);
            }
            clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, NULL);
            buildLog[buildLogSize - 1] = '\0';
            printf("Device %u build log:\n%s\n", i, buildLog);
            free(buildLog);
        }
        exit(0);
    } else {
        printf("No build errors.\n");
    }
}

//void tp4_openCL::createKernel() {
//    kernel = clCreateKernel(program, "gaussianBlurFilter", &status);
//    if (status != CL_SUCCESS) {
//        printf("clCreateKernel failed.\n");
//        exit(-1);
//    }
//
//    // Mapping the input and output arguments to the kernel
//    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &iImage);
//    status |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &iImage);
//    status |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &iFilter);
//    status |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &iFilterSize);
//    status |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &iWidth);
//
//    if (status != CL_SUCCESS) {
//        printf("clSetKernelArg failed.\n");
//        exit(-1);
//    }
//}

void tp4_openCL::cleaUp() {
    clReleaseContext(context);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(inImage);
    clReleaseMemObject(inFilter);
    clReleaseMemObject(inFilterSize);
    clReleaseMemObject(inWidth);
    clReleaseMemObject(outImage);
    clReleaseProgram(program);

}



