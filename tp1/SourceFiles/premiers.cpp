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


#define DEFAULT_THREADS_NUMBER  1
// Global shared varianles
char *gFlags;
unsigned long gMax, gCandidate;
int gCount, gAlgoType = 0;
int gThreadsNumbers;


// Initialization Mutex
pthread_mutex_t gCandidateLock = PTHREAD_MUTEX_INITIALIZER;


// Method declaration
void *invalidate_pairs_number(void *iArgs);

void invalidate_multiple(unsigned long p);

void *find_multiple(void *iArg);


int main(int argc, char *argv[]) {


    // First argument is the limit, the second is the mode of runner (sequential or parallel),
    // and the third is the number of threads to use.
    // Execute the program: ./Primes [limit] [1|seq] [nthreads]
    switch (argc) {
        case 2:
            gMax = (unsigned long) atol(argv[1]);
            gThreadsNumbers = DEFAULT_THREADS_NUMBER;
            printf("No type of algorithm given, the program will run in sequential mode.\n"
                           "The default number of threads is %i.\n", gThreadsNumbers);
            break;
        case 3:
            gMax = (unsigned long) atol(argv[1]);
            gThreadsNumbers = atoi(argv[2]);
            gAlgoType = (gThreadsNumbers==1? 0 : 1);

            break;
        default:
            // If no argument or wrong argument chain given then,
            // the minimal default search limit is 1000
            // the default number of threads is defined by macro DEFAULT_THREADS_NUMBER
            gMax = 1000;
            gThreadsNumbers = DEFAULT_THREADS_NUMBER;
            printf("No arguments Given:\n"
                           "Default limit is %ld;\n"
                           "Default mode is sequential.\n",gMax);
    }

    // Start chronometer
    Chrono lChrono(true);

    // Allocate the prime flag table in the our memory
    gFlags = (char *) calloc(gMax, sizeof(*gFlags));
    assert(gFlags != 0);
//    printf("l'adresse de l'allocation du tableau de flag est %c\n", gFlags);


    gCandidate = 3;

    // create threads
    pthread_t lIds[gThreadsNumbers];
    //printf("Starting threads\n");
    if (gAlgoType == 1) {
        for (int i = 1; i <= gThreadsNumbers; i++) {
            // printf("Starting thread number %d \n", i);
            if (i == 1) {
                // Invalidate pair numbers as primes
                pthread_create(&lIds[0], NULL, invalidate_pairs_number, NULL);
            } else {
                // Search for other primes in non-pairs numbers
                pthread_create(&lIds[i - 1], NULL, find_multiple, NULL);
            }

        }

        // wait for thread completion
        //printf("Joining threads\n");
        for (int i = 1; i <= gThreadsNumbers; i++) {
            pthread_join(lIds[i - 1], NULL);
//            printf("Joining thread number %d \n", i);
        }
    } else {
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
            // Limit printing result to the 100 first results, for console output limit
            if (gCount < 100)printf("%ld ", p);
            gCount += 1;
        }
    }
    printf("\n");
    printf("Le nombre des nombre primaires est %i \n", gCount);
    printf("Temps d'execution = %f sec\n", lChrono.get());
    // Output in the stderr the execution time
    fprintf(stderr, "Limit = %ld;\n", gMax);
    fprintf(stderr, "Number of threads = %i threads;\n", gThreadsNumbers);
    fprintf(stderr, "Temps d'execution = %f sec.\n", lChrono.get());
    fprintf(stderr, "________________________\n");
    return 0;
}

void *invalidate_pairs_number(void *) {
    for (unsigned long i = 4; i < gMax; i += 2) {
        gFlags[i]++;
    }
    pthread_exit(NULL);
}

void invalidate_multiple(unsigned long p) {
    for (unsigned long i = 3; i * p < gMax; i += 2) {
        gFlags[i * p]++;
    }
}

void *find_multiple(void *) {
    unsigned long lLimit, lCandidate = 0;
    do {
        lLimit = (unsigned long) sqrt(gMax);
        pthread_mutex_lock(&gCandidateLock);
        lCandidate = gCandidate;
        gCandidate += 2;
        pthread_mutex_unlock(&gCandidateLock);
        if (lCandidate <= lLimit) {
            if (!gFlags[lCandidate]) {
                invalidate_multiple(lCandidate);
            }
        } else {
            pthread_exit(NULL);
        }
    } while (1);
}
