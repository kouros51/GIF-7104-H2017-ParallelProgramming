//
// Created by kouros51 on 4/28/17.
//

#ifndef TP5_THERMALSIMULATION_HPP
#define TP5_THERMALSIMULATION_HPP

#include <arrayfire.h>
#include "Chrono.hpp"

class thermalSimulation {
public:
    Chrono chrono;
    unsigned long row;
    unsigned long col;
    unsigned long saveInterval;
    af::dim4 dim;
    af::array heatMap;
    af::array cellMask;
    af::array redIndexes;
    af::array blackIndexes;
    af::array image;

    thermalSimulation(long r, long c, long i):row(r), col(c), saveInterval(i), image(row, col, 3){
        std::cout<< "Creating simulation object!!"<< std::endl;
        std::cout<< "Chronometer for performance measure was initiated, Chrono=" << chrono.get() << std::endl;
        std::cout<< "# Rows: " << row << std::endl;
        std::cout<< "# Column: " << col << std::endl;
    };

    void initializeHeatMap();
    void configSimulation(const std::string configFile);
    void initMutableIndexes();
    void propagate(const float threshold);
    void saveHeatMap(const af::array changedHeatMap);
};


#endif //TP5_THERMALSIMULATION_HPP
