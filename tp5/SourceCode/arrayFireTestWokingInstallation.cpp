//
// Created by kouros51 on 4/26/17.
// To test if the Array fire installation is working
// We use a simple Hello Word program
//


#include <iostream>
#include <arrayfire.h>


using namespace af;
using namespace std;

int main (int argc,char **argv){
    try {
        unsigned int time = clock();
        int device = argc > 1 ? atoi(argv[1]) : 0 ;
        setDevice(device);
        info();
        printf("Create a 5-by-3 matrix of random floats on GPU ?????\n");
        array A = randu(5,5, f32);
        af_print(A);

        time = clock() - time ;
        printf("Executed in %f seconds.\n", ((float)time/(float)CLOCKS_PER_SEC));

    } catch (af::exception& e) {
        fprintf(stderr, "%s\n",e.what());
        throw ;
    }

    printf("\n\n=========================\n");
    cout<< "Yay This is working"<<endl;
    return 0;
}