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
#include <math.h>


#define THREADS_NUMBER  4
// Global shared varianles
char *gFlags;
unsigned long gMax;
int gCount = 0;
long gCandidate;
pthread_t gIds[THREADS_NUMBER];

// Initialization Mutex
pthread_mutex_t gCandidateLock = PTHREAD_MUTEX_INITIALIZER;

// Method declaration
void *invalidate_pairs_number(void *iArgs);

void invalidate_multiple(unsigned long p);

void *find_multiple(void *iArg);


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

    // Allocate the prime flag table in the our memory
    gFlags = (char *) calloc(gMax, sizeof(*gFlags));
    printf("l'adresse de l'allocation du tableau de flag est %ld \n", gFlags);
    assert(gFlags != 0);
    int j = 0;

    

    gCandidate = 3;

    // create threads
    //printf("Starting threads\n");
    if (THREADS_NUMBER >1) {
        for (int i = 1; i <= THREADS_NUMBER; ++i) {
           // printf("Starting thread number %d \n", i);
                // Invalidate pair numbers as primes
            if (i ==1) pthread_create(&gIds[0], NULL, invalidate_pairs_number, (void*) j);
                // Search for other primes
            else pthread_create(&gIds[i - 1], NULL, find_multiple, (void *) i);
        }
    
        // wait for thread completion
        //printf("Joining threads\n");
        for(int i=1; i<=THREADS_NUMBER; ++i) {
            void **lWorkDone = NULL;
            pthread_join(gIds[i - 1], lWorkDone);
           // printf("Joining thread number %d \n", i);
        }
    }
    else {
    // Serial basic Sieve of Eratosthenes algorithm
        for (unsigned long p = 2; p < gMax; p++) {
            if (gFlags[p] == 0) {
                // Invalidate all multiple of p
                invalidate_multiple(p);
            }
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

void *invalidate_pairs_number(void *iArgs) {
    unsigned long  lPrimeWorkDone = 0;
    for (long i = 3; i < gMax; i++) {
        if (i % 2 == 0) gFlags[i]++;
    }
    pthread_exit((void *) lPrimeWorkDone);
}

void invalidate_multiple(unsigned long p) {
    for (unsigned long i = 3; i * p < gMax; i += 2) {
        gFlags[i * p]++;
    }
}

void *find_multiple(void *iArg) {
    unsigned long lLimit, lCandidate, lPrimeWorkDone = 0;
    do {
        lLimit = sqrt(gMax);
        pthread_mutex_lock(&gCandidateLock);
        lCandidate = gCandidate;
        gCandidate += 2;
        pthread_mutex_unlock(&gCandidateLock);
        if (lCandidate <= lLimit) {
            if(!gFlags[lCandidate]){
                invalidate_multiple(lCandidate);
                //lPrimeWorkDone++;
            }
        }else{
            pthread_exit((void *) lPrimeWorkDone);
        }
    }while (1);
}
