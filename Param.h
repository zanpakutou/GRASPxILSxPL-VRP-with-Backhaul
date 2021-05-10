#ifndef PARAM_H
#define PARAM_H

#include <chrono>
#include <string>

namespace Param{
    inline int randomseed = chrono::high_resolution_clock::now().time_since_epoch().count();
    inline int seed = 123456;
    inline string instance_set = "GBJ", instance_name = "F3", instance_dir = "instance/", output, logfile = "output/run_log.csv";
    namespace LNS{
        //Control update rate of alns
        inline const int lambda = 0.2;
        //Number of removal type
        inline const int nb_removal = 6;
        //Number of insertion type
        inline const int nb_insertion = 3;
        //Number of iteration
        inline int nb_iter = 10000;
        //Sequence beetween two ALNS weight update
        inline int seqment_len = 500;
        //Cooling factor
        inline const double alpha = 0.99977;
        //Range of remove customer rate
        inline const double remove_rate_min = 0.05;
        inline const double remove_rate_max = 0.2;
        //Control diversity or intensive of removal
        inline const int Rho = 3;
        //Number of considered solutions in request removal
        inline int top_b = 500;
        //Weight adjustment for distance, capacity and route relate measure
        inline int a = 9, b = 3, c = 5;
        //Regret-k insertion parameter
        inline int k = 3;
    }
    namespace LS{
        inline double threshold = 0.2;
        inline int granularity = 10;
    }
    inline void  parse(int argc, char *argv[]){
        while (--argc){
            argv++;
            string s(argv[0]);
            if (s == "-instanceSet"){
                instance_set = string(argv[1]);
            }
            if (s == "-instanceName"){
                instance_name = string(argv[1]);
            }
            if (s == "-instanceDir"){
                instance_dir = string(argv[1]);
            }
            if (s == "-iter"){
                LNS::nb_iter = stoi(string(argv[1]));
            }
            if (s == "-sequenceLen"){
                LNS::seqment_len = stoi(string(argv[1]));
            }
            if (s == "-output"){
                output = string(argv[1]);
            }
            if (s == "-log"){
                logfile = string(argv[1]);
            }
        }
    }
}

#endif // PARAM_H
