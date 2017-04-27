/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include <arrayfire.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <algorithm>

using namespace std;
using namespace af;

void usage(char *inName) {
    cout << endl << "Usage> " << inName << ": (config_mat_file, [dimension_matrix"
            " = 100], [max_var_treshold = 1*10^-3], [iteration_image = 1], [max_"
            "cores = 1])" << endl;
    exit(1);
}

int * parse_config (string pts, int dim) {
    int p[pts.length()+1];
    p[0] = pts.length();
    for (unsigned int i = 0; i < pts.length(); ++i)
    {
        p[i] = (pts[i]);
    }
    return p;
}


int main(int argc, char* argv[]) 
{
    try
    {
        af::info();
        ifstream lConfig;
        lConfig.open(argv[1]);
        string str_config;
        int *points;
        int dim;
        int i_mask[dim*dim];
        array mask;
        
        if (argc < 2) usage(argv[0]);
        
        if (argc >=3) dim = atoi(argv[2]);
        else dim = 100;
        
        if (lConfig.is_open()) 
        {
            while(!lConfig.eof()) 
            {
                getline(lConfig, str_config);
                str_config.erase(remove(str_config.begin(), str_config.end(),' '),str_config.end());
            }
        }
        else
        {
            cerr <<"config file not open!";
            return -1;
        }
        
        points = parse_config(str_config, dim);
        fill_n(i_mask, dim*dim, 1);
        for (unsigned int i = 0; i<*points;i+=3)
        {
            i_mask[((*points+1)*dim)+(*points+2)] = 0;
        }
    }
    catch(af::exception& e) 
    {
        throw;
    }
    return 0;
}
