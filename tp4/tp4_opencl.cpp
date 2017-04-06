//
//  tp2.cpp
//  Exemple de convolution d'image avec lodepng
//
//  Créé par Julien-Charles Lévesque
//  Copyright 2015 Université Laval. Tous droits réservés.
//


#include "lodepng.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>

//#include "Chrono.hpp"

#include <CL/cl.h>
#include "PACC/Tokenizer.hpp"

using namespace std;

//Aide pour le programme
void usage(char* inName) {
    cout << endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << endl;
    exit(1);
}

//Décoder à partir du disque dans un vecteur de pixels bruts en un seul appel de fonction
void decode(const char* inFilename,  vector<unsigned char>& outImage, unsigned int& outWidth, unsigned int& outHeight)
{
    //Décoder
    unsigned int lError = lodepng::decode(outImage, outWidth, outHeight, inFilename);

    //Montrer l'erreur s'il y en a une.
    if(lError) 
        cout << "Erreur de décodage " << lError << ": " << lodepng_error_text(lError) << endl;

    //Les pixels sont maintenant dans le vecteur outImage, 4 octets par pixel, organisés RGBARGBA...
}

//Encoder à partir de pixels bruts sur le disque en un seul appel de fonction
//L'argument inImage contient inWidth * inHeight pixels RGBA ou inWidth * inHeight * 4 octets
void encode(const char* inFilename, vector<unsigned char>& inImage, unsigned int inWidth, unsigned int inHeight)
{
    //Encoder l'image
    unsigned lError = lodepng::encode(inFilename, inImage, inWidth, inHeight);

    //Montrer l'erreur s'il y en a une.
    if(lError)
        cout << "Erreur d'encodage " << lError << ": "<< lodepng_error_text(lError) << endl;
}

const int ELEMENTS = 2048;

int main(int inArgc, char *inArgv[])
{
    
    cout<< "Running Image Filtering Program"<<endl<<endl;
    
    size_t datasize = sizeof(int)*ELEMENTS;
    
    int *A, *B;
    int *C;
    
    A = (int*)malloc(datasize);
    B = (int*)malloc(datasize);
    C = (int*)malloc(datasize);
    if (A==NULL || B==NULL || C==NULL) {
        cerr<<"Momery allocation";
        exit(-1);
    }
    
    for (int i = 0; i< ELEMENTS ; i++) {
        A[i] = i;
        B[i] = i;
    }
    
    cl_int status;
    
    cl_uint numPlatforms = 0;
    
    cl_platform_id *platforms;
    
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    
    if(status != CL_SUCCESS) {
        printf("clGetPlatformIDs failed\n");
        exit(-1);
    }
    
    if(numPlatforms == 0) {
        printf("No platforms detected \n");
        exit(-1);
    }
    
    platforms = (cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));
    
    if (platforms == NULL) {
        cerr<<"Memory allocation";
        exit(-1);
    }
    
    clGetPlatformIDs(numPlatforms, platforms, NULL);
    if (status != CL_SUCCESS) {
        cout << "clGetPlatformIDs failed!" <<endl;
        exit(-1);
    }
    
    cout << numPlatforms << " platforms detected" << endl;
    
    for(unsigned int i = 0; i< numPlatforms; i++) {
        char buf[100];
        cout << "Platform "<<i<<":"<<endl;
        status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,sizeof(buf),
                buf, NULL);
        cout<<"\tVendor: "<<buf<<endl;
        status |= clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME,sizeof(buf), 
                buf, NULL);
        cout<<"\tName: "<<buf<<endl;
        
        if (status != CL_SUCCESS) {
            cout << "clGetPlatformIDs failed!" <<endl;
            exit(-1);
        }
    }
    
    cl_uint numDevices;
    cl_device_id *devices;
    
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0 , NULL, 
            &numDevices);
    
    if (status != CL_SUCCESS) {
        cout << "clGetDeviceIDs failed!" << endl;
        exit(-1);
    }
    
    if (numDevices == 0) {
        cout << "No devices detected!" << endl;
        exit (-1);
    }
    
    devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
    
    if (devices == NULL) {
        cerr << "Memory allocation";
        exit(-1);
    }
    
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, numDevices, 
            devices, NULL);
    if (status != CL_SUCCESS) {
        cout<<"clGetDeviceIDs failed!" <<endl;
        exit(-1);
    }
    cout<<endl;
    
    cout << numDevices << " devices detected!" <<endl;
    
    for(unsigned int j = 0; j< numDevices; j++) {
        char buff[100];
        cout << "Device "<<j<<":"<<endl;
        status = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(buff), 
                buff, NULL);
        cout<<"\tVendor: "<<buff<<endl;
        status |= clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(buff), 
                buff, NULL);
        cout<<"\tName: "<<buff<<endl;
        
        if (status != CL_SUCCESS) {
            cout << "clGetDeviceIDs failed!" <<endl;
            exit(-1);
        }
    }
    cout<<endl;
    
    cl_context context;
    
    context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
    if  (status != CL_SUCCESS || context == NULL) {
        cout << "clCreateContext failed" <<endl;
        exit(-1);
    }
    
    cl_command_queue cmdQueue;
    
    cmdQueue = clCreateCommandQueue (context, devices[0], 0, & status);
    if (status != CL_SUCCESS || cmdQueue == NULL) {
        cout << "clCreateCommandQueue failed!" << endl;
        exit (-1);
    }
    
    cl_mem d_A, d_B;
    cl_mem d_C;
    
    d_A = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
            datasize,A,&status);
    if (status != CL_SUCCESS || d_A == NULL) {
        cout<<"clCreateBuffer failed!"<<endl;
        exit(-1);
    }
    
    d_B = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
            datasize,B,&status);
    if (status != CL_SUCCESS || d_B == NULL) {
        cout<<"clCreateBuffer failed!"<<endl;
        exit(-1);
    }
    
    d_C = clCreateBuffer(context, CL_MEM_READ_WRITE,datasize,NULL,&status);
    if (status != CL_SUCCESS || d_C == NULL) {
        cout<<"clCreateBuffer failed!"<<endl;
        exit(-1);
    }

    cl_program program;
    
    char *source;
    const char *sourceFile = "ImageFilter.cl";
    
    FILE* file = fopen(sourceFile, "rb");
    if (file == NULL) {
        cerr<<"Couldn't read program file";
        exit(-1);
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    source = (char*)malloc(size+1);
    source[size+1]='\0';
    fread(source, sizeof(char), size, file);
    fclose(file);
    
    
    cout<<"Program source is: "<<sourceFile<<"."<<endl;
    
    program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL,
            &status);
    
    if (status != CL_SUCCESS) {
        cout<<"clCreateProgramWithSource failed!"<<endl;
        exit(-1);
    }
    
    cl_int buildErr;
    
    buildErr = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);
    
    if (buildErr != CL_SUCCESS) {
        cout<<"Program failed to build"<<endl;
        
        cl_build_status buildStatus;
        
        for (unsigned int i = 0; i < numDevices; i++) {
            clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_STATUS,
                    sizeof(cl_build_status), &buildStatus, NULL);
            if(buildStatus == CL_SUCCESS) {
                continue;
            }
        
        
            char *buildLog;

            size_t buildLogSize;

            clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_LOG, 0, 
                    NULL, &buildLogSize);
            buildLog = (char*) malloc(sizeof(buildLogSize));
            if (buildLog == NULL) {
                cerr<<"Memory Allocation";
                exit(-1);
            }
            clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_LOG,
                    buildLogSize, buildLog, NULL);
            buildLog[buildLogSize-1] = '\0';
            
            cout<< "Device "<<i<<" build log:"<<endl<<buildLog<<endl;
            
            free(buildLog);
        }
        exit(0);
    }
    else {
        cout << "No build errors"<<endl;
    }
    
    cl_kernel kernel;
    
    kernel = clCreateKernel(program, "imgfilter", &status);
    if (status != CL_SUCCESS) {
        cout << "clCreatekernel failed!"<<endl;
        exit(-1);
    }
    
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
    if (status != CL_SUCCESS) {
        cout<<"clSetKernelArg failed!"<<endl;
        exit(-1);
    }
    
    size_t globalWorkSize[1];
    globalWorkSize[0] = ELEMENTS;
    
    status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize, 
            NULL, 0, NULL, NULL);
    if(status!=CL_SUCCESS) {
        cout<<"clEnqueueNDRangeKernel failed!" <<endl;
        exit(-1);
    }
    
    clEnqueueReadBuffer(cmdQueue, d_C, CL_TRUE, 0, datasize, C, 0, NULL, NULL);
    
    bool result = true;
    
    for (int i = 0; i < ELEMENTS; i++) {
        if (C[i] != i+i) {
            result = false;
            break;
        }
    }
    if (result) {
        cout << "Output is correct!"<<endl;
    }
    else {
        cout<<"Output is incorrect"<<endl;
    }
//    if(inArgc < 3 or inArgc > 4) usage(inArgv[0]);
//    string lFilename = inArgv[1];
//    string lOutFilename;
//    if (inArgc == 4)
//        lOutFilename = inArgv[3];
//    else
//        lOutFilename = "output.png";
//
//    // Lire le noyau.
//    ifstream lConfig;
//    lConfig.open(inArgv[2]);
//    if (!lConfig.is_open()) {
//        cerr << "Le fichier noyau fourni (" << inArgv[2] << ") est invalide." << endl;
//        exit(1);
//    }
//    
//    PACC::Tokenizer lTok(lConfig);
//    lTok.setDelimiters(" \n","");
//        
//    string lToken;
//    lTok.getNextToken(lToken);
//    
//    int lK = atoi(lToken.c_str());
//    int lHalfK = lK/2;
//    
//    cout << "Taille du noyau: " <<  lK << endl;
//    
//    //Lecture du filtre
//    double* lFilter = new double[lK*lK];
//        
//    for (int i = 0; i < lK; i++) {
//        for (int j = 0; j < lK; j++) {
//            lTok.getNextToken(lToken);
//            lFilter[i*lK+j] = atof(lToken.c_str());
//        }
//    }
//
//    //Lecture de l'image
//    //Variables à remplir
//    unsigned int lWidth, lHeight; 
//    vector<unsigned char> lImage; //Les pixels bruts
//    //Appeler lodepng
//    decode(lFilename.c_str(), lImage, lWidth, lHeight);
//    
//    //Variables contenant des indices
//    int fy, fx;
//    //Variables temporaires pour les canaux de l'image
//    double lR, lG, lB;
//    for(int x = lHalfK; x < (int)lWidth - lHalfK; x++)
//    {
//        for (int y = lHalfK; y < (int)lHeight - lHalfK; y++)
//        {
//            lR = 0.;
//            lG = 0.;
//            lB = 0.;
//            for (int j = -lHalfK; j <= lHalfK; j++) {
//                fy = j + lHalfK;
//                for (int i = -lHalfK; i <= lHalfK; i++) {
//                    fx = i + lHalfK;
//                    //R[x + i, y + j] = Im[x + i, y + j].R * Filter[i, j]
//                    lR += double(lImage[(y + j)*lWidth*4 + (x + i)*4]) * lFilter[fx + fy*lK];
//                    lG += double(lImage[(y + j)*lWidth*4 + (x + i)*4 + 1]) * lFilter[fx + fy*lK];
//                    lB += double(lImage[(y + j)*lWidth*4 + (x + i)*4 + 2]) * lFilter[fx + fy*lK];
//                }
//            }
//            //Placer le résultat dans l'image.
//            lImage[y*lWidth*4 + x*4] = (unsigned char)lR;
//            lImage[y*lWidth*4 + x*4 + 1] = (unsigned char)lG;
//            lImage[y*lWidth*4 + x*4 + 2] = (unsigned char)lB;
//        }
//    }
//    
//    //Sauvegarde de l'image dans un fichier sortie
//    encode(lOutFilename.c_str(),  lImage, lWidth, lHeight);
//
//    cout << "L'image a été filtrée et enregistrée dans " << lOutFilename << " avec succès!" << endl;
//
//    delete lFilter;
    
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(d_A);
    clReleaseMemObject(d_B);
    clReleaseMemObject(d_C);
    clReleaseContext(context);
    
    free(A);
    free(B);
    free(C);
    free(source);
    free(platforms);
    free(devices);

    
    return 0;
}

