#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "Chrono.hpp"

char* gFlags;
// Programme qui trouve à l'aide du Crible d'Ératosthène,
// tous les nombres premiers inférieurs à un certain seuil
// spécifié sur la ligne de commande.
// Attention, ce programme n'est aucunement optimisé!
void * eratosthene(void* numbers){
    // Allouer le tableau des drapeaux (flags) d'invalidation
    int *lMax = (int*)numbers;

    // Une autre approche séquentiel pour l'algorithme
    int k=2;
    while (k*k<*lMax){
        int i=k*k;
        while (i<*lMax){
            gFlags[i]=1;
            i+=k;
        }
        do{
            k+=1;
        }while (gFlags[k]==1);
    }
    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    // Déterminer la limite supérieure pour la recherche;
    // par défaut, prendre 1000
    unsigned long lMax = 1000;
    int lThreads = 1;
    if (argc >= 2) {
        lMax = atol(argv[1]);
    }
    // Allouer le tableau des drapeaux (flags) d'invalidation
    char *gFlags = (char*) calloc(lMax, sizeof(*gFlags));
    assert(gFlags != 0);
    // Démarrer le chronomètre
    Chrono lChrono(true);

    // create threads
    pthread_t threads;
    printf("Starting threads");
    for(int i=1; i<=lThreads; ++i) {
        printf(" %d", i);
        pthread_create(&threads, NULL,eratosthene,(void*)lMax);
    }

    // wait for thread completion
    printf("\nJoining threads ");
    for(int i=1; i<=lThreads; ++i) {
        pthread_join(threads, NULL);
        printf(" %d", i);
    }
//    // Allouer le tableau des drapeaux (flags) d'invalidation
//    char *gFlags = (char*) calloc(lMax, sizeof(*gFlags));
//    printf("%ld",gFlags);
//    assert(gFlags != 0);


//    // Appliquer le crible d'Ératosthène
//    for (unsigned long p=2; p < lMax; p++) {
//        if (gFlags[p] == 0) {
//            // invalider tous les multiples
//            for (unsigned long i=2; i*p < lMax; i++) {
//                gFlags[i*p]++;
//            }
//        }
//    }



    // Arrêter le chronomètre
    lChrono.pause();
    int count =0;
    // Afficher les nombres trouvés à la console
    for (unsigned long p=2; p<lMax; p++) {
        if (gFlags[p] == 0){
//            printf("%ld ", p);
            count +=1;
        }
    }
    printf("\n");

    // Afficher le temps d'exécution dans le stderr
    fprintf(stderr,"Le nombre de nombre primaires est %d.",count);
    fprintf(stderr,"\n");
    fprintf(stderr, "Temps d'execution = %f sec\n", lChrono.get());

    return 0;
}
