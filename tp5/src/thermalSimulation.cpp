//
// Created by kouros51 on 4/28/17.
//

#include "headers/thermalSimulation.hpp"
#include <fstream>
#include <iomanip>

static const float maxHue = 360;
static const float blueHue = 240 / maxHue;
static const float maxT = 255;
static int imageIndex = 0;

void saveHeatMap(af::array heatMap);

void thermalSimulation::initializeHeatMap() {
    dim = af::dim4(row, col);
    heatMap = af::constant(0, dim);
    af_print(heatMap);
}

void thermalSimulation::configSimulation(const std::string configFile) {
    std::ifstream configIn;
    std::string line;

    cellMask = af::constant(false, row - 2, col - 2, b8);
    af_print(cellMask);

    configIn.open(configFile);
    if (configIn.is_open()) {
        std::cout << "Opening configuration file succeeded:" << configFile << std::endl;

        while (std::getline(configIn, line)) {
            std::istringstream iss(line);
            int configValues[2];

            for (unsigned int i = 0; i <= 2; i++) {
                iss >> configValues[i];
            }
            if ((configValues[0] >= 0) && (configValues[0] < (row)) && (configValues[1] >= 0) &&
                (configValues[1] < (col))) {
                /** Populate the heatMap by the values in the configuration file*/
                heatMap(configValues[0], configValues[1]) = configValues[2];

                /** Populate the mask immutable values depending on the configuration file*/
                if ((configValues[0] > 0) && (configValues[0] < (row - 1))) {
                    if ((configValues[1] > 0) && (configValues[1] < (col - 1))) {
                        cellMask(configValues[0] - 1, configValues[1] - 1) = true;
                    } else {
                        std::cout << "(" << configValues[0] << " " << configValues[1] << " " << configValues[2] << ")"
                                  << " ignored in populating the mask"
                                  << ", Column: " << configValues[1] << " is outside the mask matrix!" << std::endl;
                    }
                } else {
                    std::cout << "(" << configValues[0] << " " << configValues[1] << " " << configValues[2] << ")"
                              << " ignored in populating the mask"
                              << ", Row: " << configValues[0] << " is outside the mask matrix!" << std::endl;
                }
            } else {
                std::cout << "Row:" << configValues[0] << ", Column:" << configValues[1] << " is outside the matrix!"
                          << std::endl;
            }
        }
    } else {
        std::cout << "Opening configuration file failed . Check the file or the location" << std::endl;
    }
    std::cout << "===== After the configuration file======" << std::endl;
    af_print(cellMask);
    af_print(heatMap);
}

void thermalSimulation::initMutableIndexes() {

    std::cout << "===== Start Simulation======" << std::endl;
    chrono.resume();
    af::array allIndexes = af::iota(dim, af::dim4(1), u64)(af::seq(1, af::end - 1), af::seq(1, af::end - 1));
    af::array diags = af::constant(false, af::dim4(dim[0] - 2, dim[1] - 2), b8);

    diags(af::seq(0, af::end, 2), af::seq(0, af::end, 2)) = true;

    if (dim[0] > 3 && dim[1] > 3) {
        diags(af::seq(1, af::end, 2), af::seq(1, af::end, 2)) = true;
    }


    redIndexes = allIndexes(diags && !cellMask);
    blackIndexes = allIndexes(!diags && !cellMask);
    chrono.pause();
//    af_print(allIndexes);
//    af_print(redIndexes);
//    af_print(blackIndexes);
}

void thermalSimulation::propagate(const float threshold) {

    chrono.resume();

    af::array copyHeatMap = heatMap.copy();
    af::array comparingMatrix;
    float max = af::max<float>(af::max(copyHeatMap));

    while (max >= threshold) {
        copyHeatMap = heatMap.copy();
        heatMap(redIndexes) =
                (heatMap(redIndexes + 1) + heatMap(redIndexes + 1) + heatMap(redIndexes - row) +
                 heatMap(redIndexes + row)) / 4;
        heatMap(blackIndexes) =
                (heatMap(blackIndexes - 1) + heatMap(blackIndexes + 1) + heatMap(blackIndexes - row) +
                 heatMap(blackIndexes + row)) / 4;

        chrono.pause();
        saveHeatMap(heatMap);
        chrono.resume();

        af_print(heatMap);
        comparingMatrix = heatMap - copyHeatMap;
        max = af::max<float>(af::max(comparingMatrix));
    }

    chrono.pause();

    std::cout << "===== Finished simulation ======" << std::endl;
    std::cout << "Computation time: " << chrono.get() << "seconds." <<std::endl;

}


void saveHeatMap(const af::array heatMap) {

    af::array image = af::array(heatMap.dims(0), heatMap.dims(1), 3);
    image(af::span, af::span, 0) = -blueHue / maxT * heatMap + blueHue; // hue
    image(af::span, af::span, 1) = 1; // saturation max
    image(af::span, af::span, 2) = 255; // valeur max

    image = af::scale(image,25.0f,25.0f);

    image = af::hsv2rgb(image);

    std::stringstream ss;
    ss << "frames/" << std::setfill('0') << std::setw(8) << imageIndex++ << ".png";
    const std::string& fileName = ss.str();
    af::saveImage(fileName.c_str(), image);
}