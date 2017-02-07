#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Chrono.hpp"

// Programme qui trouve à l'aide du Crible d'Ératosthène,
// tous les nombres premiers inférieurs à un certain seuil
// spécifié sur la ligne de commande.
// Attention, ce programme n'est aucunement optimisé!
int main(int argc, char *argv[])
{
    // Déterminer la limite supérieure pour la recherche;
    // par défaut, prendre 1000
    unsigned long lMax = 1000;
    if (argc >= 2) {
        lMax = atol(argv[1]);
    }

    // Démarrer le chronomètre
    Chrono lChrono(true);

    // Allouer le tableau des drapeaux (flags) d'invalidation
    char *lFlags = (char*) calloc(lMax, sizeof(*lFlags));
//    printf("%ld",lFlags);
    assert(lFlags != 0);


    // Appliquer le crible d'Ératosthène
    for (unsigned long p=2; p < lMax; p++) {
        if (lFlags[p] == 0) {
            // invalider tous les multiples
            for (unsigned long i=2; i*p < lMax; i++) {
                lFlags[i*p]++;
            }
        }
    }

//    // Une autre approche séquentiel pour l'algorithme
//    int k=2;
//    while (k*k<lMax){
//        int i=k*k;
//        while (i<lMax){
//            lFlags[i]=1;
//            i+=k;
//        }
//        do{
//            k+=1;
//        }while (lFlags[k]==1);
//    }

    // Arrêter le chronomètre
    lChrono.pause();
    int count =0;
    // Afficher les nombres trouvés à la console
    for (unsigned long p=2; p<lMax; p++) {
        if (lFlags[p] == 0){
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
