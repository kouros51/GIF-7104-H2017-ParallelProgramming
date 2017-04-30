//
// Created by kouros51 on 4/26/17.
// To test if the Array fire installation is working
// We use a simple Hello Word program
//


#include <iostream>
#include <arrayfire.h>


void testMatrix();

using namespace af;
using namespace std;

int main(int argc, char **argv) {

//    int device = argc > 1 ? atoi(argv[1]) : 0;
//    setDevice(device);

    try {
        cout << "Trying CPU Backend" << endl;
        setBackend(AF_BACKEND_CPU);
        unsigned int time = clock();
        testMatrix();

        time = clock() - time;
        printf("Executed in %f seconds.\n", ((float) time / (float) CLOCKS_PER_SEC));

    } catch (af::exception &e) {
        cout << "Failed CPU Backend" << endl;
        fprintf(stderr, "%s\n", e.what());
        throw;
    }

    try {
        cout << "Trying OpenCL Backend" << endl;
        setBackend(AF_BACKEND_CUDA);
        testMatrix();

    } catch (af::exception &e) {
        cout << "Failed OpenCL Backend" << endl;
        fprintf(stderr, "%s\n", e.what());
        throw;
    }

    printf("\n\n=========================\n");
    cout << "Yay This is working" << endl;
    return 0;
}

void testMatrix() {
    info();
    printf("Create a 5-by-3 matrix of random floats on GPU ?????\n");
    array A = randu(5, 5, f32);
    af_print(A);
}