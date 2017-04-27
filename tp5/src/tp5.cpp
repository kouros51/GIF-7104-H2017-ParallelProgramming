
#include <arrayfire.h>
#include <cstdio>
#include <cstdlib>

using namespace af;

void initMutableIndexes(dim4 dim, const array& mask, array& redIndexes, array& blackIndexes) {
    auto allIndexes = iota(dim, dim4(1, 1), u64);
    af_print(allIndexes);

    auto diags = flat(allIndexes(seq(1, dim[0] - 2, 2), seq(1, dim[1] - 2, 2)));

    if (dim[0] > 3 && dim[1] > 3) {
        diags = join(0, diags, flat(allIndexes(seq(2, dim[0] - 2, 2), seq(2, dim[1] - 2, 2))));
    }

    redIndexes = sort(diags(mask(diags).copy()));

    // auto blackDiags = dim.elements() - 1 - diags;
    blackIndexes = sort((dim.elements() - 1 - diags)(mask(dim.elements() - 1 - diags).copy()));
}

void sim() {
    int n = 5;
    int m = 8;

    auto heatMap = iota(dim4(n, m), dim4(1, 1), u16);

    // auto t = iota(dim4(n, m), dim4(1, 1), u64);



    // auto heatMap = constant(0, n, m);
    // heatMap(seq(0, n - 1, n - 1), span) = 1;
    // heatMap(span, seq(0, m - 1, m - 1)) = 1;

    // auto row1 = seq(1, n - 2, 2);
    // auto col1 = seq(1, m - 2, 2);

    // auto row2 = seq(2, n - 2, 2);
    // auto col2 = seq(2, m - 2, 2);



    // auto redIndexes = join(0, flat(t(row1, col1)), flat(t(row2, col2)));

    // heatMap += 1;
    // af_print(heatMap(seq(0, n * m - 1, 2)).col(0));

    // auto rows1 = heatMap(row1, span);


    // heatMap(row1, col1) = heatMap(row1, col1 - 1.) / 4
    //                     + heatMap(row1, col1 + 1.) / 4
    //                     + heatMap(row1 - 1., col1) / 4
    //                     + heatMap(row1 + 1., col1) / 4;

    // heatMap(row2, col2) = heatMap(row2, col2 - 1.) / 4
    //                     + heatMap(row2, col2 + 1.) / 4
    //                     + heatMap(row2 - 1., col2) / 4
    //                     + heatMap(row2 + 1., col2) / 4;

    // heatMap(rouges, rouges) =


    auto mask = randu(dim4(n, m), b8);
    array redIndexes;
    array blackIndexes;
    initMutableIndexes(dim4(n, m), mask, redIndexes, blackIndexes);

    af_print(mask);
    af_print(redIndexes);
    af_print(blackIndexes);


    //af_print(redIndexes);
    //af_print(heatMap(redIndexes));

    //af_print(heatMap(sel));

    // gfor(seq i, 10, 2, 100) {
    //     heatMap(i) += 1;
    // }

    // af_print(heatMap);
}

int main(int argc, char *argv[])
{



    try {


        // Select a device and display arrayfire info
        int device = argc > 1 ? atoi(argv[1]) : 0;
        af::setDevice(device);
        af::info();

        sim();

        // printf("Create a 5-by-3 matrix of random floats on the GPU\n");
        // array A = randu(5,3, f32);
        // af_print(A);

        // printf("Element-wise arithmetic\n");
        // array B = sin(A) + 1.5;
        // af_print(B);

        // printf("Negate the first three elements of second column\n");
        // B(seq(0, 2), 1) = B(seq(0, 2), 1) * -1;
        // af_print(B);

        // printf("Fourier transform the result\n");
        // array C = fft(B);
        // af_print(C);

        // printf("Grab last row\n");
        // array c = C.row(end);
        // af_print(c);

        // printf("Scan Test\n");
        // dim4 dims(16, 4, 1, 1);
        // array r = constant(2, dims);
        // af_print(r);

        // printf("Scan\n");
        // array S = af::scan(r, 0, AF_BINARY_MUL);
        // af_print(S);

        // printf("Create 2-by-3 matrix from host data\n");
        // float d[] = { 1, 2, 3, 4, 5, 6 };
        // array D(2, 3, d, afHost);
        // af_print(D);

        // printf("Copy last column onto first\n");
        // D.col(0) = D.col(end);
        // af_print(D);

        // // Sort A
        // printf("Sort A and print sorted array and corresponding indices\n");
        // array vals, inds;
        // sort(vals, inds, A);
        // af_print(vals);
        // af_print(inds);

    } catch (af::exception& e) {

        fprintf(stderr, "%s\n", e.what());
        throw;
    }

    return 0;
}
