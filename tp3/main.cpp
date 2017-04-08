//
//  main.cpp
//

#include "Matrix.hpp"
#include "Chrono.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <mpi.h>


// Inverser la matrice par la méthode de Gauss-Jordan; implantation MPI parallèle.
void invertParallel(Matrix &iA);

using namespace std;
using namespace MPI;

size_t gPivot;
void printValarray (const valarray<double>& va, int num);

// Inverser la matrice par la méthode de Gauss-Jordan; implantation séquentielle.
void invertSequential(Matrix &iA) {

    // vérifier que la matrice est carrée
    assert(iA.rows() == iA.cols());
    // construire la matrice [A I]
    MatrixConcatCols lAI(iA, MatrixIdentity(iA.rows()));

    // traiter chaque rangée
    for (size_t k = 0; k < iA.rows(); ++k) {
        cout << "##########pour k:" << k << endl;
        // trouver l'index p du plus grand pivot de la colonne k en valeur absolue
        // (pour une meilleure stabilité numérique).
        size_t p = k;
        double lMax = fabs(lAI(k, k));
        for (size_t i = k; i < lAI.rows(); ++i) {
            if (fabs(lAI(i, k)) > lMax) {
                lMax = fabs(lAI(i, k));
                p = i;
            }
        }
        cout << "indice du pivot:" << p << endl;
        // vérifier que la matrice n'est pas singulière
        if (lAI(p, k) == 0) throw runtime_error("Matrix not invertible");

        cout << "before swap\n" << lAI.str() << endl;
        // échanger la ligne courante avec celle du pivot
        if (p != k) {
            cout << "indice du k:" << k << endl;
            lAI.swapRows(p, k);
            cout << "after swap\n" << lAI.str() << endl;
        }

        double lValue = lAI(k, k);
        for (size_t j = 0; j < lAI.cols(); ++j) {
            // On divise les éléments de la rangée k
            // par la valeur du pivot.
            // Ainsi, lAI(k,k) deviendra égal à 1.
            lAI(k, j) /= lValue;
        }
        cout << "after division\n" << lAI.str() << endl;

        // Pour chaque rangée...
        for (size_t i = 0; i < lAI.rows(); ++i) {
            if (i != k) { // ...différente de k
                // On soustrait la rangée k
                // multipliée par l'élément k de la rangée courante
                double lValue = lAI(i, k);
                cout << "lValue"<< lValue << endl;
                printValarray(lAI.getRowCopy(k),8);
                lAI.getRowSlice(i) -= lAI.getRowCopy(k) * lValue;
            }
        }
        cout << "Iteration"<< k <<", Computation\n" << lAI.str() << endl;
    }

    // On copie la partie droite de la matrice AI ainsi transformée
    // dans la matrice courante (this).
    for (unsigned int i = 0; i < iA.rows(); ++i) {
        iA.getRowSlice(i) = lAI.getDataArray()[slice(i * lAI.cols() + iA.cols(), iA.cols(), 1)];
    }
}


void printValarray (const valarray<double>& va, int num)
{
    for (int i=0; i<va.size()/num; i++) {
        for (int j=0; j<num; j++) {
            cout << va[i*num+j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}


// Multiplier deux matrices.
Matrix multiplyMatrix(const Matrix &iMat1, const Matrix &iMat2) {

    // vérifier la compatibilité des matrices
    assert(iMat1.cols() == iMat2.rows());
    // effectuer le produit matriciel
    Matrix lRes(iMat1.rows(), iMat2.cols());
    // traiter chaque rangée
    for (size_t i = 0; i < lRes.rows(); ++i) {
        // traiter chaque colonne
        for (size_t j = 0; j < lRes.cols(); ++j) {
            lRes(i, j) = (iMat1.getRowCopy(i) * iMat2.getColumnCopy(j)).sum();
        }
    }
    return lRes;
}

int main(int argc, char **argv) {

//    MPI::Init();
    srand((unsigned) time(NULL));

//    int lSize = COMM_WORLD.Get_size();
//    int lRank = COMM_WORLD.Get_rank();

    unsigned int lS = 5;
    if (argc == 2) {
        lS = atoi(argv[1]);
    }


//    if (lRank == 0) {
//        //Identify The master process
//        cout << "COMM World:\n" << lSize << endl;
//        cout << "je suis le master processus, " << lRank << endl;

        MatrixRandom lA(lS, lS);
        Matrix lB(lA);

        cout << "Matrice random:\n" << lA.str() << endl;
        cout << "Matrice random copie:\n" << lB.str() << endl;

        invertSequential(lB);

        cout << "Matrice inverse:\n" << lB.str() << endl;

//        Matrix lRes = multiplyMatrix(lA, lB);
//        cout << "Produit des deux matrices:\n" << lRes.str() << endl;
//        cout << "Erreur: " << lRes.getDataArray().sum() - lS << endl;
//    }
//    invertParallel(lB);

//    cout << "Je suis le processus" << lRank << endl;
//    MPI::Finalize();

    return 0;
}

// Inverser la matrice par la méthode de Gauss-Jordan; implantation MPI parallèle.
void invertParallel(Matrix &iA) {


    // vous devez coder cette fonction

    MatrixConcatCols lAI(iA, MatrixIdentity(iA.rows()));
    size_t lPivot;
    // traiter chaque rangée
    for (size_t k = 0; k < iA.rows(); ++k) {
        lPivot = k;
        // trouver l'index p du plus grand pivot de la colonne k en valeur absolue
        // (pour une meilleure stabilité numérique).
        double lMax = fabs(lAI(k, k));
        for (size_t i = k; i < lAI.rows(); ++i) {
            if (fabs(lAI(i, k)) > lMax) {
                lMax = fabs(lAI(i, k));
                lPivot = i;
            }
        }
    }
    MPI_Allreduce(&lPivot,&gPivot,1,MPI_DOUBLE_INT,MPI_MAXLOC,COMM_WORLD);

}
