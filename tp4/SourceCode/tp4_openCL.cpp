#include <stdio.h>
#include <CL/cl.h>
#include <CL/opencl.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>

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
        status |= clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t),
                                  &max_work_group_size, NULL);
        printf("\tCL_DEVICE_MAX_WORK_GROUP_SIZE: %zu\n", max_work_group_size);

        // Getting device Type
        cl_uint max_work_item_dimension;
        status |= clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(max_work_item_dimension),
                                  &max_work_item_dimension, NULL);
        printf("\tCL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %u\n", max_work_item_dimension);

        size_t* max_work_item_sizes = (size_t*)malloc(sizeof(size_t) * max_work_item_dimension);
        status |= clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * max_work_item_dimension, max_work_item_sizes, NULL);
        printf("\tCL_DEVICE_MAX_WORK_ITEM_SIZES:\n");
        for (size_t i = 0; i < max_work_item_dimension; ++i) {
            printf("\t\tFor dimension[%lu]:%lu\t\n", i, max_work_item_sizes[i]);
        }

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
    cmdQueue = clCreateCommandQueue(context, devices[0], 0, &status);
    if (status != CL_SUCCESS || cmdQueue == NULL) {
        printf("clCreateCommandQueueWithProperties failed.");
        exit(-1);
    }
}

void tp4_openCL::initializeBuffer(std::vector<unsigned char> *gImage, unsigned int width, unsigned int height,
                                  int filterSize, double *filter) {
    printf("Allocating buffers for the kernel.\n");
    inImage = clCreateBuffer(context, CL_MEM_READ_ONLY , sizeof(unsigned  char) * width * height, gImage, &status);
    inFilter = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * filterSize*filterSize,filter,&status);
    inFilterSize = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &filterSize, &status);
    inWidth = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &width, &status);
    outImage = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned  char) * width * height, NULL, &status);

}

char *tp4_openCL::readOpenClKernelFile() {
    printf("Reading kernel from source.\n");
    FILE *sourceFile = fopen("../filter.cl", "rb");
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

void tp4_openCL::createKernel() {
    printf("Creating the kernel.\n");
    kernel = clCreateKernel(program, "gaussianBlurFilter", &status);
    if (status != CL_SUCCESS) {
        printf("clCreateKernel failed.\n");
        exit(-1);
    }else{
        printf("Creating kernel succeeded.\n");
    }

    // Mapping the input and output arguments to the kernel
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inImage);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &inFilter);
    status |= clSetKernelArg(kernel, 2, sizeof(int), &inFilterSize);
    status |= clSetKernelArg(kernel, 3, sizeof(int), &inWidth);
    status |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &outImage);

    if (status != CL_SUCCESS) {
        printf("Mapping I/O arguments failed.\n");
        std::cout << status<<std::endl;
        exit(-1);
    }else{
        printf("Mapping I/O arguments succeeded.\n");
    }
}

void tp4_openCL::cleaUp() {
    clReleaseContext(context);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(inImage);
    clReleaseMemObject(inFilter);
    clReleaseMemObject(inFilterSize);
    clReleaseMemObject(inWidth);
    clReleaseMemObject(outImage);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

}

void tp4_openCL::runKernel(std::vector<unsigned char> *gImage, unsigned int width, unsigned int height) {
    printf("Enqueuing in the NDRange.\n");
    size_t globalWorkSize[2]={width,height};
    size_t localWorkSize[2]={8,8};
    status = clEnqueueNDRangeKernel(cmdQueue,kernel,2,NULL,globalWorkSize,localWorkSize,0,NULL,NULL);
    if(status!=CL_SUCCESS){
        printf("Enqueuing Failed!!!!.\n");
        std::cout<<status<<std::endl;
        exit(-1);
    }
    printf("Enqueuing in the NDRange succeeded.\n");
    printf("Querying the result.\n");
    status = clEnqueueReadBuffer(cmdQueue,outImage,CL_TRUE,0,sizeof(unsigned  char) * width * height,gImage,0,NULL,NULL);
    if(status!=CL_SUCCESS){
        printf("clEnqueueReadBuffer Failed!!!!.\n");
        if (status == CL_INVALID_VALUE) std::cout<<"\tErreur Code: "<<status<<std::endl;
        exit(-1);
    }
    printf("Querying the result succeeded.\n");
}



