/*
Base on: https://doi.org/10.1016/j.cor.2011.05.005
*/

#ifndef LNSMOVE_H
#define LNSMOVE_H
#include <vector>
#include "Param.h"
#include "Solution.h"

struct Graph {
    vector< vector< double > > adj_list;
    Graph();
    Graph(int nb_node);
};
namespace LNSMove  {
    inline Graph neighbor_graph, request_graph;
    void LNS_init();
    void updateLNSMove(Solution s);
    namespace Remove{
        void shawRemove(Solution& s, int gamma, int rho = Param::LNS::Rho);
        void randomRemove(Solution &s, int gamma);
        void worstRemove(Solution &s, int gamma, int rho = Param::LNS::Rho);
        void clusterRemove(Solution &s, int gamma);
        void neighborGraphRemove(Solution &s, int gamma,int rho = Param::LNS::Rho);
        void requestGraphRemove(Solution &s, int gamma, int rho = Param::LNS::Rho);
    }

    namespace Insert{
        void basicGreedyInsert(Solution& s);
        void deepGreedyInsert(Solution& s);
        void regretKInsert(Solution& s, int k);
    }
}

#endif // LNSMOVE_H
