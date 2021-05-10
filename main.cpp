#include <iostream>
#include "Solution.h"
#include "Instance.h"
#include "Model.h"
#include "Param.h"

using namespace std;

int main(int argc, char *argv[])
{
    Param::parse( argc,  argv);
    ofstream of(Param::output);
    instance = Instance(Param::instance_dir,Param::instance_set,Param::instance_name);

    cerr << instance.nb_customer << '\n';
    double st = clock();
    Solution s = ALNS::ALNS();
    double ed = clock();


    s.print(of);
    ofstream log(Param::logfile, ofstream::app);
    log << fixed <<  Param::instance_name << ',' << (ed-st)/CLOCKS_PER_SEC << ',' << s.getCost() << '\n';
    return 0;
}
