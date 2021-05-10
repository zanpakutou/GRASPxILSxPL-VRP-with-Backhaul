#include <algorithm>
#include <assert.h>
#include "LocalSearch.h"
#include "Instance.h"
#include "Route.h"
#include "Utils.h"
#include "Param.h"
#include "NeighborList.h"

using namespace std;

bool relocateMove(Route &a, Route &b){
    int capacity_a_line = 0, capacity_a_back = 0, capacity_b_line = 0, capacity_b_back = 0;
    for(int i =  0; i < (int)a.getCustomerId().size(); i++)
        if (instance.getCustomer(a, i).getType() == linehaul)
            capacity_a_line += instance.getCustomer(a, i).getQuantity();
        else if (instance.getCustomer(a, i).getType() == backhaul)
            capacity_a_back += instance.getCustomer(a, i).getQuantity();

    for(int i =  0; i < (int)b.getCustomerId().size(); i++)
        if (instance.getCustomer(b, i).getType() == linehaul)
            capacity_b_line += instance.getCustomer(b, i).getQuantity();
        else if (instance.getCustomer(b, i).getType() == backhaul)
            capacity_b_back += instance.getCustomer(b, i).getQuantity();
    int nb_linehaul = 0;

    for(int c : a.getCustomerId())
        if (instance.customers[c].getType() == linehaul)
            nb_linehaul++;
    if (nb_linehaul <= 1)
        return false;

    for(int i = 1; i < (int)a.getCustomerId().size() - 1; i++)
        for(int j = 1; j < (int)b.getCustomerId().size(); j++){     //insert i before j
            if (instance.getCustomer(b, j).getType() == linehaul &&
                instance.getCustomer(a, i).getType() == backhaul)
                    continue;
            if (instance.getCustomer(b, j - 1).getType() == backhaul &&
                instance.getCustomer(a, i).getType() == linehaul)
                    continue;
            if (instance.getCustomer(a, i).getType() == linehaul)
                if (capacity_b_line + instance.getCustomer(a, i).getQuantity() > instance.cap)
                    continue;
            if (instance.getCustomer(a, i).getType() == backhaul)
                if (capacity_b_back + instance.getCustomer(a, i).getQuantity() > instance.cap)
                    continue;
            double delta_cost = - getDist(a.getCustomerId()[i - 1], a.getCustomerId()[i])
                                - getDist(a.getCustomerId()[i], a.getCustomerId()[i + 1])
                                - getDist(b.getCustomerId()[j - 1], b.getCustomerId()[j])
                                + getDist(a.getCustomerId()[i - 1], a.getCustomerId()[i + 1])
                                + getDist(a.getCustomerId()[i], b.getCustomerId()[j])
                                + getDist(a.getCustomerId()[i], b.getCustomerId()[j - 1]);

            if (-delta_cost > Param::LS::threshold) {
                int relocated_customer = a.getCustomerId()[i];
                a.getCustomerId().erase(a.getCustomerId().begin() + i);
                b.getCustomerId().insert(b.getCustomerId().begin() + j, relocated_customer);
                return true;
            }
        }
    return false;
}

bool swapMove(Route& a, Route& b){
   int capacity_a_line = 0, capacity_a_back = 0, capacity_b_line = 0, capacity_b_back = 0;

    for(int i =  0; i < (int)a.getCustomerId().size(); i++)
        if (instance.getCustomer(a, i).getType() == linehaul)
            capacity_a_line += instance.getCustomer(a, i).getQuantity();
        else if (instance.getCustomer(a, i).getType() == backhaul)
            capacity_a_back += instance.getCustomer(a, i).getQuantity();

    for(int i =  0; i < (int)b.getCustomerId().size(); i++)
        if (instance.getCustomer(b, i).getType() == linehaul)
            capacity_b_line += instance.getCustomer(b, i).getQuantity();
        else if (instance.getCustomer(b, i).getType() == backhaul)
            capacity_b_back += instance.getCustomer(b, i).getQuantity();

    for(int i = 1; i < (int)a.getCustomerId().size() - 1; i++)
        for(int j = 1; j < (int)b.getCustomerId().size() - 1; j++){     //swap i and j
            if (instance.getCustomer(b, j + 1).getType() == linehaul &&
                instance.getCustomer(a, i).getType() == backhaul)
                    continue;
            if (instance.getCustomer(b, j - 1).getType() == backhaul &&
                instance.getCustomer(a, i).getType() == linehaul)
                    continue;
            if (instance.getCustomer(a, i + 1).getType() == linehaul &&
                instance.getCustomer(b, j).getType() == backhaul)
                    continue;
            if (instance.getCustomer(a, i - 1).getType() == backhaul &&
                instance.getCustomer(b, j).getType() == linehaul)
                    continue;
            //Calculate new capacity
            int new_cap_a_line = capacity_a_line, new_cap_a_back = capacity_a_back;
            int new_cap_b_line = capacity_b_line, new_cap_b_back = capacity_b_back;
            Customer ai = instance.getCustomer(a, i), bj = instance.getCustomer(b, j);
            if (ai.getType() == linehaul)
                new_cap_a_line-= ai.getQuantity(), new_cap_b_line+= ai.getQuantity();
            if (ai.getType() == backhaul)
                new_cap_a_back-= ai.getQuantity(), new_cap_b_back+= ai.getQuantity();
            if (bj.getType() == linehaul)
                new_cap_b_line-= bj.getQuantity(), new_cap_a_line+= bj.getQuantity();
            if (bj.getType() == backhaul)
                new_cap_b_back-= bj.getQuantity(), new_cap_a_back+= bj.getQuantity();


            if (new_cap_a_back > instance.cap || new_cap_a_line > instance.cap
                || new_cap_b_back > instance.cap || new_cap_b_line > instance.cap)
                    continue;
            double delta_cost = - getDist(a.getCustomerId()[i - 1], a.getCustomerId()[i])
                                - getDist(a.getCustomerId()[i], a.getCustomerId()[i + 1])
                                - getDist(b.getCustomerId()[j - 1], b.getCustomerId()[j])
                                - getDist(b.getCustomerId()[j], b.getCustomerId()[j + 1])
                                + getDist(a.getCustomerId()[i - 1], b.getCustomerId()[j])
                                + getDist(b.getCustomerId()[j], a.getCustomerId()[i + 1])
                                + getDist(b.getCustomerId()[j - 1], a.getCustomerId()[i])
                                + getDist(a.getCustomerId()[i], b.getCustomerId()[j + 1]);


            if (-delta_cost > Param::LS::threshold) {
                //cerr << a.getCost() << ' ' << b.getCost() << '\n';
                swap(a.getCustomerId()[i], b.getCustomerId()[j]);
                //cerr << a.getCost() << ' ' << b.getCost() << '\n';
                return true;
            }
        }
    return false;
}

///TSP optimizer
bool twoOptMove(Route& a)
{
    //cerr << "2opt\n";
    //-> i -> j -> i+1 -> j+1 ->
    for(int i = 0; i < (int)a.getCustomerId().size() - 1; i++)
        for(int j = i + 2; j < (int)a.getCustomerId().size() - 1; j++){
            if (instance.getCustomer(a, j).getType() != linehaul &&
                instance.getCustomer(a, i + 1).getType() != backhaul)
                continue;
            double delta_cost = -getDist(a.getCustomerId()[i], a.getCustomerId()[i + 1])
                                -getDist(a.getCustomerId()[j], a.getCustomerId()[j + 1])
                                +getDist(a.getCustomerId()[i], a.getCustomerId()[j])
                                +getDist(a.getCustomerId()[i + 1], a.getCustomerId()[j + 1]);

            if (-delta_cost > Param::LS::threshold) {
                reverse(a.getCustomerId().begin() + i + 1, a.getCustomerId().begin() + j + 1);
                return true;
            }
        }
    return false;
}
bool orOptMove(Route& a)
{
    //cerr << "or\n";
    //-> i-1 -> i+2 -> j -> i -> i+1 -> j+1 ->
    for(int i = 1; i < (int)a.getCustomerId().size() - 2; i++)
        for(int j = 1; j < (int)a.getCustomerId().size() - 1; j++){
            if (i - 1 <= j && j <= i + 2)
                continue;
             if (instance.getCustomer(a, i).getType() == linehaul &&
                 instance.getCustomer(a, j).getType() == backhaul)
                continue;
            if (instance.getCustomer(a, i + 1).getType() == backhaul &&
                 instance.getCustomer(a, j + 1).getType() == linehaul)
                continue;
            double delta_cost = - getDist(a.getCustomerId()[i - 1], a.getCustomerId()[i])
                                - getDist(a.getCustomerId()[i + 1], a.getCustomerId()[i + 2])
                                - getDist(a.getCustomerId()[j], a.getCustomerId()[j + 1])
                                + getDist(a.getCustomerId()[i - 1], a.getCustomerId()[i + 2])
                                + getDist(a.getCustomerId()[j], a.getCustomerId()[i])
                                + getDist(a.getCustomerId()[i + 1], a.getCustomerId()[j + 1]);

            if (-delta_cost > Param::LS::threshold) {
                int node_i = a.getCustomerId()[i], node_i1 = a.getCustomerId()[i + 1];
                /*
                cerr << "cost\n" << ' ' << i << ' ' << j << '\n';
                cerr << a.getCost() << '\n';
                for(auto c : a.getCustomerId()) cerr << c << ' '; cerr << '\n';
                */
                a.getCustomerId().erase(a.getCustomerId().begin() + i, a.getCustomerId().begin() + i + 2);
                if (i < j) j-= 2;
                a.getCustomerId().insert(a.getCustomerId().begin() + j + 1, node_i);
                a.getCustomerId().insert(a.getCustomerId().begin() + j + 2, node_i1);
                /*
                for(auto c : a.getCustomerId()) cerr << c << ' '; cerr << '\n';
                cerr  << a.getCost() << '\n';
                */
                return true;
            }
        }
    return false;
}
bool relocateIntraMove(Route& a)
{   //cerr << "relocateintra\n";
    //-> j -> i -> j + 1 ->
    for(int i = 1; i < (int)a.getCustomerId().size() - 1; i++)
        for(int j = 1; j < (int)a.getCustomerId().size() - 1; j++){
            if (i == j + 1 || i == j) continue;
            if (instance.getCustomer(a, j).getType() == backhaul &&
                instance.getCustomer(a, i).getType() == linehaul)
                continue;
            if (instance.getCustomer(a, j + 1).getType() == linehaul &&
                instance.getCustomer(a, i).getType() == backhaul)
                continue;
            double delta_cost = -getDist(a.getCustomerId()[i - 1], a.getCustomerId()[i])
                                -getDist(a.getCustomerId()[i], a.getCustomerId()[i + 1])
                                -getDist(a.getCustomerId()[j], a.getCustomerId()[j + 1])
                                +getDist(a.getCustomerId()[i - 1], a.getCustomerId()[i + 1])
                                +getDist(a.getCustomerId()[j], a.getCustomerId()[i])
                                +getDist(a.getCustomerId()[i], a.getCustomerId()[j + 1]);

            if (-delta_cost > Param::LS::threshold) {
                int node_i = a.getCustomerId()[i];
                /*cerr << "cost\n" << ' ' << i << ' ' << j << '\n';
                cerr << a.getCost() << '\n';
                for(auto c : a.getCustomerId()) cerr << c << ' '; cerr << '\n';*/
                a.getCustomerId().erase(a.getCustomerId().begin() + i);
                if (i < j) j--;
                a.getCustomerId().insert(a.getCustomerId().begin() + j + 1, node_i);
                /*
                for(auto c : a.getCustomerId()) cerr << c << ' '; cerr << '\n';
                cerr  << a.getCost() << '\n';*/
                return true;
            }
        }
    return false;
}
bool TSP_optimizer(Route& a)
{
    if ( orOptMove(a) || relocateIntraMove(a) || twoOptMove(a) ) return true;
    return false;
}
void LocalSearch(Solution& s)
{
    vector< int > route(instance.nb_customer, -1);
    for(int i = 0; i < (int)s.getRoutes().size(); i++)
        for(int c : s.getRoutes()[i].getCustomerId())
            route[c] = i;
    vector< pair< int, int > > neighbor_route;
    for(int i = 0; i < instance.nb_customer; i++)
        for(int j : NeighborList::neighbor[i]){
            if (route[i] < route[j])
                neighbor_route.push_back(make_pair(route[i], route[j]));
            if (route[j] < route[i])
                neighbor_route.push_back(make_pair(route[i], route[j]));
        }
    sort(neighbor_route.begin(), neighbor_route.end());
    neighbor_route.resize(unique(neighbor_route.begin(), neighbor_route.end()) - neighbor_route.begin());

    bool success = true;

    do {
        success = false;
        double x = s.getCost(), y;
        //cerr << s.getCost() << '\n';
        for(auto p : neighbor_route){
                int i = p.first;
                int j = p.second;
                //cerr << "relocate\n";
                success|= relocateMove(s.getRoutes()[i], s.getRoutes()[j]);
                success|= relocateMove(s.getRoutes()[j], s.getRoutes()[i]);
                //cerr << "swap\n";
                success|= swapMove(s.getRoutes()[i], s.getRoutes()[j]);
               // cerr << "tspoptimizer\n";
                if (success)
                    success|= TSP_optimizer(s.getRoutes()[i]);
                success|= TSP_optimizer(s.getRoutes()[j]);
            }
        y = s.getCost();
        //cerr << success << '\n';
        if (success) assert(x > y);
    } while (success);
}
