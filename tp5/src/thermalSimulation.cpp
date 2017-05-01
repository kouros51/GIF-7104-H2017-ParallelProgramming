//
// Created by kouros51 on 4/28/17.
//

#include "headers/thermalSimulation.hpp"
#include <fstream>
#include <iomanip>
#include <limits>

static const float maxHue = 360;
static const float blueHue = 240 / maxHue;
static const float maxT = 255;
static int imageIndex = 0;

void thermalSimulation::saveHeatMap(const af::array changedHeatMap) {

    image(af::span, af::span, 0) = -blueHue / maxT * changedHeatMap + blueHue; // hue
    image(af::span, af::span, 1) = 1; // saturation max
    image(af::span, af::span, 2) = 255; // valeur max

    image = af::hsv2rgb(image);

    std::stringstream ss;
    ss << "frames/" << std::setfill('0') << std::setw(8) << imageIndex++ << ".png";
    const std::string& fileName = ss.str();
    af::saveImage(fileName.c_str(), image);
}


void thermalSimulation::initializeHeatMap() {
    dim = af::dim4(row, col);
    heatMap = af::constant(0, dim);
    // af_print(heatMap);
}

void thermalSimulation::configSimulation(const std::string configFile) {
    std::ifstream configIn;
    std::string line;

    cellMask = af::constant(false, row - 2, col - 2, b8);
    // af_print(cellMask);

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
    // af_print(cellMask);
    // af_print(heatMap);
}

void thermalSimulation::initMutableIndexes() {

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

    auto heatMapCopy = heatMap.copy();
    af::array* deltaHeatMaps[2] = {&heatMap, &heatMapCopy};

    float max = std::numeric_limits<float>::infinity();

    // auto redNorth = redIndexes - 1;
    // auto redSouth = redIndexes + 1;
    // auto redEast = redIndexes + row;
    // auto redWest = redIndexes - row;

    // auto blackNorth = blackIndexes - 1;
    // auto blackSouth = blackIndexes + 1;
    // auto blackEast = blackIndexes + row;
    // auto blackWest = blackIndexes - row;


    for (unsigned long i = 0, c = 0; max >= threshold ; c++, i = 1 - i) {
        auto currentHeatMap = deltaHeatMaps[i];
        auto nextHeatMap = deltaHeatMaps[1 - i];

        if (!(c % saveInterval)) {
            chrono.pause();
            saveHeatMap(*currentHeatMap);
            chrono.resume();
        }

        (*nextHeatMap)(redIndexes) =    ((*currentHeatMap)(redIndexes - 1) +
                                        (*currentHeatMap)(redIndexes + 1) +
                                        (*currentHeatMap)(redIndexes - row) +
                                        (*currentHeatMap)(redIndexes + row)) / 4;

        (*nextHeatMap)(blackIndexes) =  ((*nextHeatMap)(blackIndexes - 1) +
                                        (*nextHeatMap)(blackIndexes + 1) +
                                        (*nextHeatMap)(blackIndexes - row) +
                                        (*nextHeatMap)(blackIndexes + row)) / 4;

        max = af::max<float>(*nextHeatMap - *currentHeatMap);
    }

    chrono.pause();

    std::cout << "===== Finished simulation ======" << std::endl;
    std::cout << "Computation time: " << chrono.get() << "seconds." <<std::endl;

}
