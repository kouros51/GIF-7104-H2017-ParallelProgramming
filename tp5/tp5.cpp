//
// Created by kouros51 on 4/28/17.
//

#include <arrayfire.h>
#include <istream>
#include <iomanip>
#include <fstream>

#include "src/headers/thermalSimulation.hpp"

using namespace af;
using namespace std;

static const float maxHue = 360;
static const float blueHue = 240 / maxHue;
static const float maxT = 255;
static size_t imageIndex = 0;


//void initMutableIndexes(dim4 dim, const array &mask, array &redIndexes, array &blackIndexes);
void saveHeatMap(af::array heatMap);

void usage(char *inName);

int main(int argc, char *argv[]) {
    /** Argument organisation of the array of argument "argv"
     *  is mapped like:
     *  1- configFIle
     *  2- dimension of the simulation matrix, default = 100 * 100
     *  3- threshold to stop simulation, default = 10E-2
     *  4- # minimal iteration iteration before saving in an image, default 1
     *  5- # cores to use in the simulation */

    string configFile;
    static float threshold;
    static int minimumIteration;
    static float cores;

    try {
        if (argc < 2) {
            fprintf(stderr, "Error in one of the argument of the program");
            usage(argv[0]);
        } else{

            threshold = atof(argv[4]);

            /** Print Device information*/
            info();

            /** Load configuration file */
            configFile = argv[1];

            /** Simulation steps using a thermalSimulation object.
             *  This object manage all the aspects of the simulation.
             *  The times spend in loading configuration saving images
             *  or making the resulting video is ignored.
             *  Only the computation time is calculated.
             */

            /** Initiatlization part*/
            thermalSimulation simulation{atoi(argv[2]), atoi(argv[3])};
            simulation.initializeHeatMap();
            simulation.configSimulation(configFile);

            /** Simulation part*/
            simulation.initMutableIndexes();
            simulation.propagate(threshold);


            /** Visualization, produce images, making video parts*/
        }

    } catch (af::exception &e) {

        fprintf(stderr, "%s\n", e.what());
        throw;
    }

    return 0;

}

void usage(char *inName) {
    std::cout << std::endl << "Usage> " << inName << ": (config_mat_file, [dimension_matrix"
            " = 100 100], [max_var_treshold = 1*10^-2], [iteration_image = 1], [max_"
            "cores = 1])" << std::endl;
    exit(1);
}