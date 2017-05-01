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

    try {
        vector<string> args(argv, argv + argc);

        auto optionRef = std::find(args.begin(), args.end(), "-f");
        string configFile = optionRef != args.end() ? optionRef[1] : "../configs/configFile";

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
                    << "Threshold: " << threshold << std::endl
                    << "Configuration file location: " << threshold << std::endl;

        /** Cleaning working directory */
        std::cout << "Cleaning working directories.\n" << std::endl;
        system("rm -f frames/*");
        system("rm -f video/*");

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
        std::cout << "======= Start Simulation \t======" << std::endl;
        simulation.initMutableIndexes();
        simulation.propagate(threshold);
        generateVideo();

    } catch (af::exception &e) {

        fprintf(stderr, "%s\n", e.what());
        throw;
    }

    return 0;
}

void usage(char *inName) {
    std::cout << std::endl << "Usage> " << inName << " -d [défault : 100 100] -s [default : 0.01] -f [default : ../configs/configFile] -i  i [default : 1]" << std::endl;
    exit(1);
}

void generateVideo(){
    system("ffmpeg -loglevel quiet -r 10 -i frames/%8d.png -y video/video.mp4");
}
