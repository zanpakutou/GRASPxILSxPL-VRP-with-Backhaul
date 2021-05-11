#include "PathRelinking.h"
#include "Instance.h"
#include "Utils.h"
#include "LocalSearch.h"
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

int common_subset(vector< int > a,vector< int > b)
{
    int cnt = 0;

    map< int , int > M;
    for(int i : a) M[i]++;
    for(int i : b) M[i]++;
    for(auto i : M)
        if (i.second == 2)
            cnt++;
    return cnt;
}
// A/(A and B)
vector< int > versus_common(vector< int > a, vector< int > b)
{
    vector< int > result;
    map< int , int > M;
    for(int i : b) M[i]++;
    for(int i : a)
        if (M[i] == 0)
            result.push_back(i);
    return result;
}

//O(n * m^2)
int similarity(Solution s, Solution t, vector< int >& matching)
{
    vector< Route > route_source = s.getRoutes();
    vector< Route > route_target = t.getRoutes();

    int n = route_source.size(), m = route_target.size();

    vector< pair< int, pair< int, int > > > edges;
    for(int i = 0; i < n; i++)
        for(int j = 0; j < m; j++){
            int cnt = common_subset(route_source[i].getCustomerId(), route_target[j].getCustomerId());
            edges.emplace_back(cnt, make_pair(i, j));
        }

    sort(edges.begin(), edges.end());
    reverse(edges.begin(), edges.end());

    vector< bool > picked_source(n, false), picked_target(m, false);

    int value = 0;
    matching.clear();
    matching = vector< int >(n, 0);
    for(auto i : edges)
        if (picked_source[i.second.first] == false && picked_target[i.second.second] == false)
            value+= i.first, picked_source[i.second.first] = true, picked_target[i.second.second] = true, matching[i.second.first] = i.second.second;

    return  instance.nb_customer -  value;
}

/*

*/
void addToElitePool(Solution new_s)
{
    similarity_threshold = instance.nb_customer/6;
    if (eliteSet.empty()) {
        eliteSet.push_back(new_s);
        return;
    }

    vector< int > __;

    int new_cost = new_s.getCost(), worst_objective = 0, min_distance = 1e8;
    int worse_min_distace_index = -1, _ = 1e9;
    bool check = true;
    for(Solution s : eliteSet){
        int s_cost = s.getCost();
        int s_similar = similarity(new_s, s, __);
        worst_objective = max(worst_objective, s_cost),
        min_distance = min(min_distance, s_similar);
        if (new_cost > s_cost - 0.01 && s_similar < similarity_threshold)
            check = false;

        if (s_cost > new_cost)
            if (worse_min_distace_index == -1 || s_similar < _){
                worse_min_distace_index = 1;
                _ = s_similar;
            }
    }
    if ((int)eliteSet.size() < pool_size){
        if (min_distance > similarity_threshold)
            eliteSet.push_back(new_s);
        return;
    }

    // check = true : solution better than S sastisfy the similarity constraint
    vector< Solution > new_pool;
    if (check){
        //Find worse solution to remove
        bool availble = false;
        for(int i = 0; i < (int)eliteSet.size(); i++){
            Solution s = eliteSet[i];
            if (s.getCost() > new_cost && similarity(new_s, s, __) < similarity_threshold)
                availble = true;
            else new_pool.push_back(s);
        }
        if (availble){
            new_pool.push_back(new_s);
            eliteSet = new_pool;
            return;
        }
        //Remove worse closest solution
        for(int i = 0; i < (int)eliteSet.size(); i++){
            Solution s = eliteSet[i];
            int s_similar = similarity(new_s, s, __);
            if (s_similar == _){
                eliteSet.erase(eliteSet.begin() + i);
                break;
            }
        }
        eliteSet.push_back(new_s);
    }

    return;
}

bool bestInsertion(int node, Route& r)
{
    if (instance.customers[node].getType() == linehaul){
        int best_pos = -1;
        double best_dist = 0;
        for(int i = 1; i < (int)r.getCustomerId().size() - 1; i++){
            if (instance.customers[r.getCustomerId()[i - 1]].getType() == linehaul)
                if (best_pos == -1 || getDist(r.getCustomerId()[i - 1],node) + getDist(r.getCustomerId()[i + 1],node) < best_dist)
                    {
                        best_pos = i;
                        best_dist = getDist(r.getCustomerId()[i - 1],node) + getDist(r.getCustomerId()[i + 1],node);
                    }
        }
        if (best_pos != -1)
            r.getCustomerId().insert(r.getCustomerId().begin() + best_pos, node);
    } else {
        int best_pos = -1;
        double best_dist = 0;
        for(int i = 1; i < (int)r.getCustomerId().size() - 1; i++){
            if (instance.customers[r.getCustomerId()[i + 1]].getType() == backhaul)
                if (best_pos == -1 || getDist(r.getCustomerId()[i - 1],node) + getDist(r.getCustomerId()[i + 1],node) < best_dist)
                    {
                        best_pos = i;
                        best_dist = getDist(r.getCustomerId()[i - 1],node) + getDist(r.getCustomerId()[i + 1],node);
                    }
        }
        if (best_pos != -1)
            r.getCustomerId().insert(r.getCustomerId().begin() + best_pos, node);
    }
    return r.isFeasible();
}
//s and t has same number of route
Solution pathRelinking(Solution s, Solution t)
{
    vector< int > matching;
    int distance = instance.nb_customer * 2 - similarity(s, t, matching);
    //Calculating NF
    vector< int > NF;
    for(int k = 0; k < (int)matching.size(); k++){
        vector< int > uncommon_vertexs = versus_common(s.getRoutes()[k].getCustomerId(), t.getRoutes()[matching[k]].getCustomerId());
        for(int i : uncommon_vertexs)
            NF.push_back(i);
    }
    //Calculating path
    map <int, int > M;
    for(int r = 0; r < (int)t.getRoutes().size(); r++)
        for(auto i : t.getRoutes()[r].getCustomerId())
            M[i] = r;

    Solution trace_best = s, s_middle = s;
    //Move
    random_shuffle(NF.begin(), NF.end());
    while(!NF.empty()){
        //move i to route M(i)
        int i = NF.back();
        NF.pop_back();
        Route r = s_middle.getRoutes()[M[i]];
        bool is_fea  = bestInsertion(i, r);
        s_middle.getRoutes()[M[i]] = r;
        if (s_middle.isFeasible() && s_middle.getCost() < trace_best.getCost() - 0.01)
            trace_best = s_middle;
    }
    if (trace_best.isFeasible() && trace_best.getCost() < s.getCost() - 0.01){
        LocalSearch(trace_best);
        return trace_best;
    }   else return s;
}

void getBestPathRelinking(Solution &s){
    if (eliteSet.empty()) return;
    Solution new_solution;

    for(Solution elite : eliteSet){
        //cerr << 0 << '\n';
        if ((int)s.getRoutes().size() == (int)elite.getRoutes().size()){
            //cerr << 1 << '\n';
            new_solution = pathRelinking(s, elite);
            //cerr << 2 << '\n';
            if (new_solution.isFeasible() && new_solution.getCost() < s.getCost() - 0.1)
                s = new_solution;
            //cerr << 3 << '\n';
        }
    }
}























