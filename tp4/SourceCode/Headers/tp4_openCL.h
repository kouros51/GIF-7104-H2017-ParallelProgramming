//
// Created by kouros51 on 4/7/17.
//

#ifndef TP4_TP4_OPENCL_H
#define TP4_TP4_OPENCL_H


class tp4_openCL {
public:
    // return value for most OpenCL functions
    cl_int status = NULL;
    cl_uint numPlatforms = 0;
    cl_platform_id *platforms = NULL;
    cl_uint numDevices = 0;
    cl_device_id *devices = NULL;
    cl_context context;
    cl_command_queue cmdQueue;
    cl_mem inImage, inFilter, inFilterSize, inWidth;
    cl_mem outImage;
    cl_program program;
    cl_int buildErr;
    cl_kernel kernel;
public:
    void initialize();

    void getPlatformsInfo();

    void getDevicesInfo();

    void initializeBuffer(std::vector<unsigned char> gImage, unsigned int height, unsigned int width, int filterSize,
                          double *filter);

    void initializeContext();

    void initializeCommandQueue();

    void initializeProgram(char *string);

    char *readOpenClKernelFile();

    void buildProgram();

    void createKernel();

    void cleaUp();
};


#endif //TP4_TP4_OPENCL_H
