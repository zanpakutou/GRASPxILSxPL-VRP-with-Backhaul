#ifndef MODEL_H
#define MODEL_H
#include "Solution.h"
#include <vector>

namespace Greedy
{
    Solution regretConstruction();
}
namespace ALNS{
    inline vector< vector< double > > weight;
    void init();
    pair< int , int > selects();
    Solution ALNS();
}

#endif // MODEL_H
