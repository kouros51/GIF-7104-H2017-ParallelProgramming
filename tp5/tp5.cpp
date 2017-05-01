//
// Created by kouros51 on 4/28/17.
//

#include <arrayfire.h>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include "src/headers/thermalSimulation.hpp"

using namespace af;
using namespace std;

void usage(char *inName);

void generateVideo();

int main(int argc, char *argv[]) {
    /** Argument organisation of the array of argument "argv"
     *  is mapped like:
     *  1- configFIle
     *  2- dimension of the simulation matrix, default = 100 * 100
     *  3- threshold to stop simulation, default = 10E-2
     *  4- # minimal iteration iteration before saving in an image, default 1
     *  5- # cores to use in the simulation */


    static int minimumIteration;

    vector<string> args(argv, argv + argc);
    auto optionRef = std::find(args.begin(), args.end(), "-f");

    try {
        if (optionRef == args.end()) {
            fprintf(stderr, "You need to include the configuration file with option f.");
            usage(argv[0]);

        } else {
            string configFile = optionRef[1];

            optionRef = std::find(args.begin(), args.end(), "-d");
            long rows = optionRef != args.end() ? atol(optionRef[1].c_str()) : 100;
            long cols = optionRef != args.end() ? atol(optionRef[2].c_str()) : 100;

            optionRef = std::find(args.begin(), args.end(), "-s");
            float threshold = optionRef != args.end() ? atof(optionRef[1].c_str()) : 0.01f;

            optionRef = std::find(args.begin(), args.end(), "-i");
            long saveInterval = optionRef != args.end() ? atol(optionRef[1].c_str()) : 1;

            std::cout   << "Number of rows: " << rows << std::endl
                        << "Number of columns: " << cols << std::endl
                        << "Frame save interval: " << saveInterval << std::endl
                        << "Threshold: " << threshold << std::endl;

            /** Print Device information*/
            info();

            /** Simulation steps using a thermalSimulation object.
             *  This object manage all the aspects of the simulation.
             *  The times spend in loading configuration saving images
             *  or making the resulting video is ignored.
             *  Only the computation time is calculated.
             */

            /** Initiatlization part*/
            thermalSimulation simulation{rows, cols, saveInterval};

            simulation.initializeHeatMap();
            simulation.configSimulation(configFile);

            /** Simulation part*/
            std::cout << "===== Start Simulation ======" << std::endl;
            simulation.initMutableIndexes();
            simulation.propagate(threshold);
            generateVideo();
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

void generateVideo(){
    system("ffmpeg -loglevel quiet -r 10 -i frames/%8d.png -y video/video.mp4");
}
