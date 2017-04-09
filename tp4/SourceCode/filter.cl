#pragma OPENCL EXTENSION cl_khr_fp64: enable
__kernel void gaussianBlurFilter(
        __global unsigned char* iImage,
        __global double* iFilter,
        const int iFilterSize,
        const int iWidth,
        __global unsigned char* ouImage
){
    unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);
    int halfFilterSize,fy,fx;
    double lR, lG, lB,tmp;

    lR = 0.;
    lG = 0.;
    lB = 0.;
    halfFilterSize = iFilterSize/2;

    for (int j = -halfFilterSize; j <= halfFilterSize; j++) {
        fy = j + halfFilterSize;
        for (int i = -halfFilterSize; i <= halfFilterSize; i++) {
            fx = i + halfFilterSize;
            //R[x + i, y + j] = Im[x + i, y + j].R * Filter[i, j]
            lR += iImage[(y + j) * iWidth * 4 + (x + i) * 4] * iFilter[fx + fy * iFilterSize];
            lG += iImage[(y + j) * iWidth * 4 + (x + i) * 4 + 1] * iFilter[fx + fy * iFilterSize];
            lB += iImage[(y + j) * iWidth * 4 + (x + i) * 4 + 2] * iFilter[fx + fy * iFilterSize];
        }
    }
    //Placer le résultat dans l'image.
    ouImage[y * iWidth * 4 + x * 4] = (unsigned char) lR;
    ouImage[y * iWidth * 4 + x * 4 + 1] = (unsigned char) lG;
    ouImage[y * iWidth * 4 + x * 4 + 2] = (unsigned char) lB;
};