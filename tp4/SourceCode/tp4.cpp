
#include "Headers/lodepng.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <CL/cl.h>
#include "Headers/tp4_openCL.h"
#include "Headers/Chrono.hpp"
#include "PACC/Tokenizer.hpp"
#include "openacc.h"


using namespace std;

void usage(char *inName);

void decode(const char *inFilename, vector<unsigned char> &outImage, unsigned int &outWidth, unsigned int &outHeight);

void encode(const char *inFilename, vector<unsigned char> &inImage, unsigned int inWidth, unsigned int inHeight);

void sequentialRunner(vector<unsigned char> vector1, unsigned int i, unsigned int i1, int i2, double *pDouble);

vector<unsigned char> openClRunner(vector<unsigned char> i, unsigned int i1, unsigned int i2, int i3, double *pDouble);

void openAccRunner(vector<unsigned char> vector1, unsigned int i, unsigned int i1, int i2, double *pDouble);

int main(int argc, char **argv) {



    if (argc < 3 or argc > 4) usage(argv[0]);
    string lFilename = argv[1];
    string lOutFilename;
    if (argc == 4)
        lOutFilename = argv[3];
    else
        lOutFilename = "output.png";

    // Lire le noyau.
    ifstream lConfig;
    lConfig.open(argv[2]);
    if (!lConfig.is_open()) {
        cerr << "Le fichier noyau fourni (" << argv[2] << ") est invalide." << endl;
        exit(1);
    }

    PACC::Tokenizer lTok(lConfig);
    lTok.setDelimiters(" \n", "");

    string lToken;
    lTok.getNextToken(lToken);

    int lK = atoi(lToken.c_str());
    int lHalfK = lK / 2;

    cout << "Taille du noyau: " << lK << endl;

    //Lecture du filtre
    double *lFilter = new double[lK * lK];

    for (int i = 0; i < lK; i++) {
        for (int j = 0; j < lK; j++) {
            lTok.getNextToken(lToken);
            lFilter[i * lK + j] = atof(lToken.c_str());
        }
    }

    //Lecture de l'image
    //Variables à remplir
    unsigned int lWidth, lHeight;
    vector<unsigned char> lImage; //Les pixels bruts
    //Appeler lodepng
    decode(lFilename.c_str(), lImage, lWidth, lHeight);


    // Start Chrono
    Chrono lChrono(true);

    lImage=openClRunner(lImage, lWidth, lHeight, lK, lFilter);
//    openAccRunner(lImage, lWidth, lHeight, lK, lFilter);
//    sequentialRunner(lImage, lWidth, lHeight, lK, lFilter);

    lChrono.pause();

    printf("Temps d'execution = %f sec.\n", lChrono.get());

    //Sauvegarde de l'image dans un fichier sortie
    encode(lOutFilename.c_str(), lImage, lWidth, lHeight);

    cout << "L'image a été filtrée et enregistrée dans " << lOutFilename << " avec succès!" << endl;

    delete [] lFilter;


    return 0;
}

//Aide pour le programme
void usage(char *inName) {
    cout << endl << "Utilisation> " << inName << " [-opencl|-openacc] fichier_image fichier_noyau [fichier_sortie=output.png]" << endl;
    exit(1);
}

//Décoder à partir du disque dans un vecteur de pixels bruts en un seul appel de fonction
void decode(const char *inFilename, vector<unsigned char> &outImage, unsigned int &outWidth, unsigned int &outHeight) {
    //Décoder
    unsigned int lError = lodepng::decode(outImage, outWidth, outHeight, inFilename);

    //Montrer l'erreur s'il y en a une.
    if (lError)
        cout << "Erreur de décodage " << lError << ": " << lodepng_error_text(lError) << endl;

    //Les pixels sont maintenant dans le vecteur outImage, 4 octets par pixel, organisés RGBARGBA...
}

//Encoder à partir de pixels bruts sur le disque en un seul appel de fonction
//L'argument inImage contient inWidth * inHeight pixels RGBA ou inWidth * inHeight * 4 octets
void encode(const char *inFilename, vector<unsigned char> &inImage, unsigned int inWidth, unsigned int inHeight) {
    //Encoder l'image
    unsigned lError = lodepng::encode(inFilename, inImage, inWidth, inHeight);

    //Montrer l'erreur s'il y en a une.
    if (lError)
        cout << "Erreur d'encodage " << lError << ": " << lodepng_error_text(lError) << endl;
}

// Sequential Runner for The GaussianBluFilter
void sequentialRunner(vector<unsigned char> lImage, unsigned int lWidth, unsigned int lHeight, int lK, double *lFilter) {
    cout<<"****Sequential runner for the blur filter****"<<endl;
    //Variables contenant des indices
    int fy, fx, lHalfK;
    //Variables temporaires pour les canaux de l'image
    double lR, lG, lB;
    lHalfK = lK/2;
    // For more measure accuracy repeat the filter several time
    for (unsigned int repeat=0; repeat<1;repeat++)
    {
        for (int x = lHalfK; x < (int) lWidth - lHalfK; x++) {
            for (int y = lHalfK; y < (int) lHeight - lHalfK; y++) {
                lR = 0.;
                lG = 0.;
                lB = 0.;
                for (int j = -lHalfK; j <= lHalfK; j++) {
                    fy = j + lHalfK;
                    for (int i = -lHalfK; i <= lHalfK; i++) {
                        fx = i + lHalfK;
                        //R[x + i, y + j] = Im[x + i, y + j].R * Filter[i, j]
                        lR += double(lImage[(y + j) * lWidth * 4 + (x + i) * 4]) * lFilter[fx + fy * lK];
                        lG += double(lImage[(y + j) * lWidth * 4 + (x + i) * 4 + 1]) * lFilter[fx + fy * lK];
                        lB += double(lImage[(y + j) * lWidth * 4 + (x + i) * 4 + 2]) * lFilter[fx + fy * lK];
                    }
                }
                //Placer le résultat dans l'image.
                lImage[y * lWidth * 4 + x * 4] = (unsigned char) lR;
                lImage[y * lWidth * 4 + x * 4 + 1] = (unsigned char) lG;
                lImage[y * lWidth * 4 + x * 4 + 2] = (unsigned char) lB;
            }
        }
    }

}

// OpenCL Runner for The GaussianBluFilter
vector<unsigned char>
openClRunner(vector<unsigned char> inLImage, unsigned int lWidth, unsigned int lHeight, int lK, double *inLFilter) {
    cout<<"**** OpenCL runner for the blur filter ****"<<endl;
    // OpenCL implementation
    tp4_openCL tp4_opencl;
    tp4_opencl.initialize();

    cout<<"==> Platforms and devices informations:"<<endl;
    tp4_opencl.getPlatformsInfo();
    tp4_opencl.getDevicesInfo();

    tp4_opencl.initializeContext();
    tp4_opencl.initializeCommandQueue();
    tp4_opencl.initializeBuffer(&inLImage,lWidth,lHeight,lK,inLFilter);


    char *kernelSource = tp4_opencl.readOpenClKernelFile();
    tp4_opencl.initializeProgram(kernelSource);
    tp4_opencl.buildProgram();
    tp4_opencl.createKernel();
    tp4_opencl.runKernel(&inLImage,lWidth,lHeight);
    cout<<"==> Cleaning up."<<endl;
    tp4_opencl.cleaUp();
}

void openAccRunner(vector<unsigned char> lImage, unsigned int lWidth, unsigned int lHeight, int lK, double *lFilter) {
    cout<<"****OpenACC runner for the blur filter****"<<endl;
    
    //Variables contenant des indices
    int fy, fx;
    //Variables temporaires pour les canaux de l'image
    double lRGB[3];
    unsigned int lDim[3];
    lDim[0] = lHeight; lDim[1] = lWidth; lDim[2] = lK/2;
    // For more measure accuracy repeat the filter several time
    //for (unsigned int repeat=0; repeat<1;repeat++)
    //{
        #pragma acc data copy(lImage) pcreate(lRGB) copyin (lDim, fy, fx)
        {
            #pragma acc parallel
            {
                for (unsigned int x = lDim[2]; x < lDim[1] - lDim[2]; x++) {

                    #pragma acc loop private(lRGB)
                    for (unsigned int y = lDim[2]; y < lDim[0] - lDim[2]; y++) {
                    	lRGB[0] = 0.; lRGB[1] = 0; lRGB[2] = 0.;

                        for (int j = -lDim[2]; j <= lDim[2]; j++) {
                            fy = j + lDim[2];

                            #pragma acc loop private(lRGB)
                            for (int i = -lDim[2]; i <= lDim[2]; i++) {
                                fx = i + lDim[2];
                                //R[x + i, y + j] = Im[x + i, y + j].R * Filter[i, j]
                                lRGB[0] += double(lImage[(y + j) * lDim[1] * 4 + (x + i) * 4]) * lFilter[fx + fy * lK];
                                lRGB[1] += double(lImage[(y + j) * lDim[1] * 4 + (x + i) * 4 + 1]) * lFilter[fx + fy * lK];
                                lRGB[2] += double(lImage[(y + j) * lDim[1] * 4 + (x + i) * 4 + 2]) * lFilter[fx + fy * lK];
                            }
                        }
                        //Placer le résultat dans l'image.
                        lImage[y * lDim[1] * 4 + x * 4] = (unsigned char) lRGB[0];
                        lImage[y * lDim[1] * 4 + x * 4 + 1] = (unsigned char) lRGB[1];
                        lImage[y * lDim[1] * 4 + x * 4 + 2] = (unsigned char) lRGB[2];
                    }
                }
            }
        }
    //}
}
