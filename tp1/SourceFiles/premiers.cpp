/*  premiers.cpp
    Based on the first algorithm for the Sieve of Eratosthenes
    proposed by Pr. Marc Parizeau, we present our attempt of
    optimizing the first algorithm then parallelize it using pthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "Chrono.hpp"

//Global shared varianles
char *gFlags;
unsigned long gMax;
int gCount = 0;

// Method declaration
void invalidate_pairs_number();

void invalidate_multiple(unsigned long p);

//void * eratosthene(void* numbers){
//    // Allouer le tableau des drapeaux (flags) d'invalidation
//    long gMax = (long)numbers;
//
//    while (gK*gK<gMax){
//        int i=gK*gK;
//        while (i<gMax){
//            pthread_mutex_lock(&gMutex);
//            if (gFlags[i]==0){
//                gFlags[i]=1;
//                i+=gK;
//            }
//            pthread_mutex_unlock(&gMutex);
//        }
//        do{
//            gK+=1;
//        }while (gFlags[gK]==1);
//    }
//    pthread_exit(NULL);
//}
int main(int argc, char *argv[]) {
    // If no argument given then,
    // the minimal default search limit is 1000
    gMax = 1000;
    //
    if (argc >= 2) {
        gMax = atol(argv[1]);
    }


    // Start chronometer
    Chrono lChrono(true);

//    // create threads
//    pthread_t lIds[lThreads];
//    printf("Starting threads");
//    for(int i=1; i<=lThreads; ++i) {
//        printf(" %d", i);
//        pthread_create(&lIds[i-1], NULL,eratosthene,(void*)gMax);
//    }
//
//    // wait for thread completion
//    printf("\nJoining threads ");
//    for(int i=1; i<=lThreads; ++i) {
//        pthread_join(lIds[i-1], NULL);
//        printf(" %d", i);
//    }

    // Allocate the prime flag table in the our memory
    gFlags = (char *) calloc(gMax, sizeof(*gFlags));
    printf("l'adresse de l'allocation du tableau de flag est %ld \n", gFlags);
    assert(gFlags != 0);

    // Invalidate pair numbers as primes
    invalidate_pairs_number();

    // Serial basic Sieve of Eratosthenes algorithm
    for (unsigned long p = 2; p < gMax; p++) {
        if (gFlags[p] == 0) {
            // Invalidate all multiple of p
            invalidate_multiple(p);
        }
    }



    // Stop Chronometer
    lChrono.pause();


    // Count primes numbers and output them on the console
    for (unsigned long p = 2; p < gMax; p++) {
        if (gFlags[p] == 0) {
            if (gCount < 20)printf("%ld ", p);
            gCount += 1;
        }
    }
    printf("\n");
    printf("Le nombre des nombre primaires est %i \n", gCount);
    // Output in the stderr the execution time
    fprintf(stderr, "\n");
    fprintf(stderr, "Temps d'execution = %f sec\n", lChrono.get());

    return 0;
}

void invalidate_pairs_number() {
    for (long i = 3; i < gMax; i++) {
        if (i % 2 == 0) gFlags[i]++;
    }
}

void invalidate_multiple(unsigned long p) {
    for (unsigned long i = 3; i * p < gMax; i += 2) {
        gFlags[i * p]++;
    }
}
