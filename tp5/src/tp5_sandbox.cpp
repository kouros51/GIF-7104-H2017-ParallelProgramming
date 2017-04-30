
#include <arrayfire.h>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace af;

static const float maxHue = 360;
static const float blueHue = 240 / maxHue;
static const float maxT = 255;
static size_t imageIndex = 0;

void saveHeatMap(array heatMap) {

    auto image = array(heatMap.dims(0), heatMap.dims(1), 3);
    image(span, span, 0) = -blueHue / maxT * heatMap + blueHue; // hue
    image(span, span, 1) = 1; // saturation max
    image(span, span, 2) = 255; // valeur max

    image = hsv2rgb(image);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(8) << imageIndex++ << ".png";
    const std::string &fileName = ss.str();
    saveImage(fileName.c_str(), image);
}

// En entree, le mask est l'ensemble des cases rouges et noirs sans la frontiere (Donc de taille n - 2 par m - 2)
// Si une valeur est immuable alors la case est a vrai/1
void initMutableIndexes(dim4 dim, const array &mask, array &redIndexes, array &blackIndexes) {

    array allIndexes = iota(dim, dim4(1), u64)(seq(0, end - 1), seq(0, end - 1));
    auto diags = constant(false, dim4(dim[0] - 2, dim[1] - 2), b8);

    diags(seq(0, end, 2), seq(0, end, 2)) = true;

    if (dim[0] > 3 && dim[1] > 3) {
        diags(seq(1, end, 2), seq(1, end, 2)) = true;
    }

    af_print(allIndexes)
    redIndexes = allIndexes(diags && !mask);
    blackIndexes = allIndexes(!diags && !mask);
}

void sim() {
    int n = 8;
    int m = 6;

    auto heatMap = range(dim4(n, m)) * n / (n - 1);

    saveHeatMap(heatMap);

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


    auto mask = randu(dim4(n - 2, m - 2), b8);
    array redIndexes;
    array blackIndexes;
    initMutableIndexes(dim4(n, m), mask, redIndexes, blackIndexes);

    af_print(mask);
    af_print(heatMap);
    af_print(redIndexes);
    af_print(blackIndexes);

    heatMap(redIndexes) =
            (heatMap(redIndexes - 1) + heatMap(redIndexes + 1) + heatMap(redIndexes - n) + heatMap(redIndexes + n)) / 4;
    heatMap(blackIndexes) =
            (heatMap(blackIndexes - 1) + heatMap(blackIndexes + 1) + heatMap(blackIndexes - n) +
             heatMap(blackIndexes + n)) / 4;
    af_print(heatMap(redIndexes));
    af_print(heatMap(blackIndexes));

//    bool stopCondition = false;
//    while(!stopCondition){
//
//    }

//    af_print(heatMap);

    //af_print(redIndexes);
    //af_print(heatMap(redIndexes));

    //af_print(heatMap(sel));

    // gfor(seq i, 10, 2, 100) {
    //     heatMap(i) += 1;
    // }

    // af_print(heatMap);
}

int main(int argc, char *argv[]) {


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

    } catch (af::exception &e) {

        fprintf(stderr, "%s\n", e.what());
        throw;
    }

    return 0;
}
