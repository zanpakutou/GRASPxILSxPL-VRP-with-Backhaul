#ifndef  NEIGHTBOR_LIST_H
#define  NEIGHTBOR_LIST_H
#include <vector>
#include "Instance.h"
#include "Utils.h"
#include "Param.h"

using namespace std;

namespace NeighborList{
    inline vector< vector< int > > neighbor;
    inline void buildNeightborList(){
        neighbor = vector< vector< int > >(instance.nb_customer, vector< int >());
        for(int i = 0; i < instance.nb_customer; i++)
            for(int j = 0; j < instance.nb_customer; j++)
                if (i != j) neighbor[i].push_back(j);

        for(int i = 0; i < instance.nb_customer; i++)
            sort(neighbor[i].begin(), neighbor[i].end(),
             [i](const int & a, const int & b) -> bool
            { return getDist(i, a) < getDist(i, b); });

        for(int i = 0; i < instance.nb_customer; i++)
            if((int)neighbor[i].size() > Param::LS::granularity)
                neighbor[i].erase(neighbor[i].begin() + Param::LS::granularity, neighbor[i].end());
    }
}

#endif
