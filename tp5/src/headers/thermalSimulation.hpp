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
    unsigned int row;
    unsigned int col;
    af::dim4 dim;
    af::array heatMap;
    af::array cellMask;
    af::array redIndexes;
    af::array blackIndexes;

    thermalSimulation(unsigned int r, unsigned int c):row(r), col(c){
        std::cout<< "Creating simulation object!!"<< std::endl;
        std::cout<< "Chronometer for performance measure was initiated, Chrono=" << chrono.get() << std::endl;
        std::cout<< "# Rows: " << row << std::endl;
        std::cout<< "# Column: " << col << std::endl;
    };

    void initializeHeatMap();
    void configSimulation(const std::string configFile);
    void initMutableIndexes();
    void propagate(const float threshold);
};


#endif //TP5_THERMALSIMULATION_HPP
