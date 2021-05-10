#include "Model.h"
#include "Utils.h"
#include "LNSMove.h"
#include "LocalSearch.h"
#include "NeighborList.h"
#include "PathRelinking.h"
#include <vector>
#include <map>
#include <iostream>
#include <assert.h>
#include <math.h>

using namespace Param;

void ALNS::init()
{
    ALNS::weight = vector< vector< double > >(LNS::nb_removal, vector< double >(LNS::nb_insertion, 1.0/(LNS::nb_removal * LNS::nb_insertion)));
}
pair< int , int > ALNS::selects(){
    double y = randomNumber() % 10000/ 10000.0, q = 0;
    for(int i = 0;i < LNS::nb_removal; i++)
        for(int j = 0; j < LNS::nb_insertion; j++){
            q+= ALNS::weight[i][j];
            if (q > y) return make_pair(i, j);
        }
    return make_pair(randomNumber() % LNS::nb_removal, randomNumber() % LNS::nb_insertion);
}

void applyLNSMove(Solution& s, pair< int , int > _move)
{
    //cerr << _move.first << ' ' << _move.second << '\n';
    int gamma = randomNumber() % int(instance.nb_customer * (Param::LNS::remove_rate_max - Param::LNS::remove_rate_min) + 1)
                + instance.nb_customer * Param::LNS::remove_rate_min;
    if (_move.first == 0) LNSMove::Remove::shawRemove(s, gamma);
    if (_move.first == 1) LNSMove::Remove::randomRemove(s, gamma);
    if (_move.first == 2) LNSMove::Remove::clusterRemove(s, gamma);
    if (_move.first == 3) LNSMove::Remove::worstRemove(s, gamma);
    if (_move.first == 4) LNSMove::Remove::neighborGraphRemove(s, gamma);
    if (_move.first == 5) LNSMove::Remove::requestGraphRemove(s, gamma);
    //cerr << "removed\n";
    if ((int)s.getRoutes().size() < instance.nb_route){
        Route empty_route;
        empty_route.getCustomerId().push_back(instance.depot.getId());
        empty_route.getCustomerId().push_back(instance.depot.getId());
        while((int)s.getRoutes().size() < instance.nb_route)
            s.getRoutes().push_back(empty_route);
    }
    if (_move.second == 0) LNSMove::Insert::basicGreedyInsert(s);
    if (_move.second == 1) LNSMove::Insert::deepGreedyInsert(s);
    if (_move.second == 2) LNSMove::Insert::regretKInsert(s, Param::LNS::k);
    //cerr << "insert\n";
}

Solution Greedy::regretConstruction(){
    Solution s;
    cerr << "Begin greedy\n";
    Route empty_route;
    empty_route.getCustomerId().push_back(instance.depot.getId());
    empty_route.getCustomerId().push_back(instance.depot.getId());
    s.getRoutes().push_back(empty_route);
    do{
        s.getRoutes().push_back(empty_route);
        LNSMove::Remove::randomRemove(s, instance.nb_customer * 0.7);
        LNSMove::Insert::deepGreedyInsert(s);
        int nbCustomer = 0;
        for(Route r : s.getRoutes())
            nbCustomer += (int)r.getCustomerId().size() -  2;
        cerr << nbCustomer << '\n';
        if (nbCustomer >= instance.nb_customer - 1) break;
    } while (!s.isFeasible());
    cerr << "info " << fixed << s.getRoutes().size() << ' ' << s.getCost() << '\n';
    cerr <<"Greedy done\n";
    //assert(0);
    return s;
}

Solution ALNS::ALNS(){
    cerr << "Begin ALNS\n";
    Solution best_solution;
    ALNS::init();
    LNSMove::LNS_init();
    NeighborList::buildNeightborList();
    double best_cost = INT_MAX;
    double T;
    vector< vector< double > > score, frequency;
    score = frequency = vector< vector< double > >(Param::LNS::nb_removal,vector< double >(Param::LNS::nb_insertion, 0));
    cerr << "Prepare ALNS done\n";

    Solution s = Greedy::regretConstruction();


    T = -0.25 * s.getCost()/ log(0.5);   //T = temprature which accept a worse solution hav w deviation with 0.5 probability;

    for(int i = 0; i < LNS::nb_iter; i++){
        if (i % LNS::seqment_len == 0)
            cerr << "Iter " << i << '\n' << best_cost << '\n';
        double reward = 0;
        double y = (randomNumber() % 10000) / 10000.0;
        pair< int , int > move_type = selects();
        Solution _s = s;
        applyLNSMove(_s, move_type);

        LocalSearch(_s);

        double before_cost = _s.getCost();
        addToElitePool(_s);
        getBestPathRelinking(_s);
        double after_cost = _s.getCost();

        if (before_cost > after_cost + 0.01) {
            cerr << "PL improved " << before_cost << ' ' << after_cost << '\n';
        }

        double old_cost = s.getCost();
        double new_cost = _s.getCost();


        //Update current solutoin with SA acceptance
        if (new_cost < old_cost) {
            s = _s;
            reward = 2;
        }
        if (new_cost > old_cost){
            if (y < exp(-(new_cost - old_cost)/T)){
                s = _s;
                reward = 1;
            }
        }

        //Update best solution
        if (new_cost < best_cost && s.isFeasible()){
            best_solution = s;
            reward = 3;
            best_cost = best_solution.getCost();
        }
        //cerr << best_cost << '\n';
        LNSMove::updateLNSMove(s);
        //Update temperature
        T = T * LNS::alpha;
        //cerr << T << '\n';
        //Adjust weight
        score[move_type.first][move_type.second]+= reward;
        frequency[move_type.first][move_type.second]+= 1;

        if (i % Param::LNS::seqment_len == 0){
            double sum_score = 0;
            //Calculate the mean of scores
            for(int i_remove = 0; i_remove < Param::LNS::nb_removal; i_remove++)
                for(int i_insert = 0; i_insert < Param::LNS::nb_insertion; i_insert++){
                    if (frequency[i_remove][i_insert] > 0)
                        score[i_remove][i_insert]/= frequency[i_remove][i_insert];
                    sum_score+= score[i_insert][i_remove ];
                }
            //Normalize
            double a = 0;
            for(int i_remove = 0; i_remove < Param::LNS::nb_removal; i_remove++)
                for(int i_insert = 0; i_insert < Param::LNS::nb_insertion; i_insert++)
                    score[i_remove][i_insert]/= sum_score, a+= score[i_remove][i_insert];

            //Update weight
            for(int i_remove = 0; i_remove < Param::LNS::nb_removal; i_remove++)
                for(int i_insert = 0; i_insert < Param::LNS::nb_insertion; i_insert++){
                    weight[i_remove][i_insert] = (1 - LNS::lambda)*weight[i_remove][i_insert]+LNS::lambda*score[i_remove][i_insert];
                    score[i_remove][i_insert] = 0;
                    frequency[i_remove][i_insert] = 0;
                }
        }
    }
    return best_solution;
}
