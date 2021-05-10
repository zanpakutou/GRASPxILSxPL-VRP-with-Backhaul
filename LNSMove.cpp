#include "Instance.h"
#include "LNSMove.h"
#include "Route.h"
#include "Solution.h"
#include "Utils.h"
#include "Param.h"
#include <vector>
#include <algorithm>
#include <map>
#include <random>
#include <limits>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace LNSMove;

vector< Solution > request_graph_pool;
double max_distance = 0;
int max_capacity_different = 0;

Graph::Graph():
    adj_list(vector< vector< double > > ())
{};

Graph::Graph(int nb_node):
    adj_list(vector< vector< double > > (nb_node, vector< double> (nb_node, max_distance)))
{
    for(int i = 0; i < instance.nb_customer; i++) this->adj_list[i][i] = 0;
}
void LNSMove::LNS_init(){
    neighbor_graph = Graph(instance.nb_customer);
    request_graph = Graph(instance.nb_customer);
    request_graph_pool.clear();
    max_distance = 0;
    max_capacity_different = 0;

    //Pre-compute
    for(int i = 0; i < instance.nb_customer; i++)
        for(int j = i + 1; j < instance.nb_customer; j++){
            max_distance = max(max_distance, getDist(i, j));
            max_capacity_different = max(max_capacity_different,
                                         abs(instance.customers[i].getQuantity() - instance.customers[j].getQuantity()));
        }
}
vector< int > calculateRequestBank(Solution& s){
    //cerr << 1;
    vector< bool > is_exist(instance.nb_customer);
    for(int i = 0; i < instance.nb_customer; i++)
        is_exist[i] = false;

    int nb_element = instance.nb_customer;

    for(Route r : s.getRoutes())
        for(int c : r.getCustomerId()){
            if (!is_exist[c]) nb_element--;
            is_exist[c] = true;
        }
    vector< int > request_bank(nb_element, 0);
    for(int i = 0; i < instance.nb_customer; i++)
        if (!is_exist[i] && i != instance.depot.getId())
                request_bank[--nb_element] = i;
    return request_bank;
}
void removeCustomer(Solution& s, int customer_id)
{
    bool removed = false;
    for(int i = 0; i <  (int)s.getRoutes().size(); i++){
        Route &r = s.getRoutes()[i];
        if (removed) break;
        if (customer_id == instance.depot.getId()) continue;
        for(int i = 0; i <  (int)r.getCustomerId().size(); i++)
            if (r.getCustomerId()[i] == customer_id){
                r.getCustomerId().erase(r.getCustomerId().begin() + i);
                removed = true;
                break;
            }
    }
    for(int i = 0; i < (int)s.getRoutes().size(); i++)
        if ((int)s.getRoutes()[i].getCustomerId().size() <= 2)
            s.getRoutes().erase(s.getRoutes().begin() + i);
}
void Remove::shawRemove(Solution &s, int gamma, int rho)
{
    vector< int > removed_customer, request_bank, customer_list, route_id(instance.nb_customer, -1);
    int r, i; double y;
    request_bank = calculateRequestBank(s);
    removed_customer = request_bank;
    for(int i = 0; i < (int)s.getRoutes().size(); i++)
        for(int c : s.getRoutes()[i].getCustomerId())
            route_id[c] = i;

    if (request_bank.empty()){
        r = randomCustomer();
        removed_customer.push_back(r);
        removeCustomer(s, r);
    }
    while((int)removed_customer.size() < gamma){
        r = removed_customer[randomNumber() % (int)removed_customer.size()];
        customer_list = s.getCustomerIds();
        auto relateMeasure = [r,route_id](int i) -> double {
            return Param::LNS::a * getDist(r, i) / max_distance
            + Param::LNS::b * abs(instance.customers[r].getQuantity() - instance.customers[i].getQuantity())
                    / max_capacity_different
            + Param::LNS::c * (route_id[i] != route_id[r] ? 1 : 0);
        };
        sort(customer_list.begin(), customer_list.end(),
             [relateMeasure](const int & a, const int & b) -> bool
            { return relateMeasure(a) < relateMeasure(b); });
        y = (randomNumber() % 7000) / 10000.0;
        i = floor(pow(y, rho) * (int)customer_list.size());
        //cerr << y << ' ' << rho << ' ' << i << ' ' << customer_list.size() << '\n';
        removed_customer.push_back(customer_list[i]);
        removeCustomer(s, customer_list[i]);
    }
}
void Remove::randomRemove(Solution &s, int gamma)
{
    vector< int > removed_customer, request_bank, customer_list;
    int r;
    request_bank = calculateRequestBank(s);
    removed_customer = request_bank;

    while((int)removed_customer.size() < gamma){
        customer_list = s.getCustomerIds();
        r = customer_list[randomNumber() % (int)customer_list.size()];


        removed_customer.push_back(r);
        removeCustomer(s, r);
    }
}

void Remove::worstRemove(Solution &s, int gamma, int rho){
    vector< int > removed_customer, request_bank, customer_list, worse_cost(instance.nb_customer, -1);
    int r, i; double y;
    request_bank = calculateRequestBank(s);
    removed_customer = request_bank;

    if (request_bank.empty()){
        r = randomCustomer();
        removed_customer.push_back(r);
        removeCustomer(s, r);
    }
    while((int)removed_customer.size() < gamma){
        r = removed_customer[randomNumber() % (int)removed_customer.size()];

        customer_list = s.getCustomerIds();
        for(Route r : s.getRoutes())
            for(int i = 1; i < (int)r.getCustomerId().size() - 1; i++)
                worse_cost[r.getCustomerId()[i]] = getDist(r.getCustomerId()[i - 1],r.getCustomerId()[i + 1])
                                -getDist(r.getCustomerId()[i - 1],r.getCustomerId()[i])
                                -getDist(r.getCustomerId()[i],r.getCustomerId()[i + 1]);
        sort(customer_list.begin(), customer_list.end(),
             [worse_cost](const int & a, const int & b) -> bool
            { return worse_cost[a] < worse_cost[b]; });
        y = (randomNumber() % 7000) / 10000.0;
        i = floor(pow(y, rho) * (int)customer_list.size());
        removed_customer.push_back(customer_list[i]);
        removeCustomer(s, customer_list[i]);
    }
}
void Remove::clusterRemove(Solution &s, int gamma)
{
    vector< int > removed_customer, request_bank, customer_list, route_id(instance.nb_customer, -1);
    int r, r_prior = -1, i;
    double current_distance;
    request_bank = calculateRequestBank(s);
    removed_customer = request_bank;

    while((int)removed_customer.size() < gamma){
        for(int i = 0; i < (int)s.getRoutes().size(); i++)
            for(int c : s.getRoutes()[i].getCustomerId())
                route_id[c] = i;

        if (removed_customer.empty())
            r = randomNumber() % (int)s.getRoutes().size();
        else {
            i = removed_customer[randomNumber() % (int)removed_customer.size()];
            current_distance = max_distance;
            for(int j = 0; j < instance.nb_customer; j++)
                if (getDist(i, j) < current_distance && route_id[i] != route_id[j] && route_id[j] != r_prior)
                    if (route_id[j] != -1 && j != instance.depot.getId()){
                        current_distance = getDist(i, j);
                        r = route_id[j];
                    }
        }
        Route _r = s.getRoutes()[r];
        if ((int)_r.getCustomerId().size() == 0) {
            s.getRoutes().erase(s.getRoutes().begin() + r);
            continue;
        }

        vector< int > cluster[2];
        pair< int, int > removed = make_pair(_r.getCustomerId()[0], _r.getCustomerId()[1]);
        int cluster_id = 0;
        for(int i = 2; i < (int)_r.getCustomerId().size() - 1; i++)
            if (getDist(_r.getCustomerId()[i - 1], _r.getCustomerId()[i]) < getDist(removed.first, removed.second)){
                removed.first = _r.getCustomerId()[i - 1];
                removed.second = _r.getCustomerId()[i];
            }

        cluster[cluster_id].push_back(_r.getCustomerId()[0]);
        for(int i = 1; i < (int)_r.getCustomerId().size(); i++){
            if (_r.getCustomerId()[i] == removed.second && _r.getCustomerId()[i - 1] == removed.first){
                cluster_id = (cluster_id + 1) % 2;
                continue;
            }
            cluster[cluster_id].push_back(i);
        }
        cluster_id = randomNumber() % 2;
        shuffle(cluster[cluster_id].begin(), cluster[cluster_id].end(), mt_rand);
        int len = min(gamma - (int)removed_customer.size() + 1, (int)cluster[cluster_id].size());
        for(int i = 0; i < len; i++){
            removed_customer.push_back(cluster[cluster_id][i]);
            removeCustomer(s, cluster[cluster_id][i]);
        }
        r_prior = r;
    }
}

void Remove::neighborGraphRemove(Solution& s, int gamma, int rho)
{
    vector< int > removed_customer, request_bank, customer_list;
    vector< double > score(instance.nb_customer, 0);

    int i; double y;
    request_bank = calculateRequestBank(s);
    removed_customer = request_bank;
    customer_list = s.getCustomerIds();
    //Calculate score for each customer using the neighbor graph
    for(int i = 0; i < instance.nb_customer; i++)
        for(int j = 0; j < instance.nb_customer; j++)
            score[i]+= neighbor_graph.adj_list[i][j];

    while((int)removed_customer.size() < gamma){
        //Sort the score with descending order
        sort(customer_list.begin(), customer_list.end(),
             [score](const int & a, const int & b) -> bool
            { return score[a] > score[b]; });
        //Choose random number y, i = floor(y^rho * |L|)
        y = (randomNumber() % 7000) / 10000.0;
        i = floor(pow(y, rho) * (int)customer_list.size());
        removed_customer.push_back(customer_list[i]);
        //Update the score to the customer close to L_i
        for(int adj = 0; adj < instance.nb_customer; adj++)
            score[adj]-= neighbor_graph.adj_list[customer_list[i]][adj];
        //Remove
        removeCustomer(s, customer_list[i]);
    }
}

void Remove::requestGraphRemove(Solution& s, int gamma, int rho)
{
    vector< int > removed_customer, request_bank, customer_list;
    int r, i; double y;
    request_bank = calculateRequestBank(s);
    removed_customer = request_bank;

    if (request_bank.empty()){
        r = randomCustomer();
        removed_customer.push_back(r);
        removeCustomer(s, r);
    }
    while((int)removed_customer.size() < gamma){
        r = removed_customer[randomNumber() % (int)removed_customer.size()];
        customer_list = s.getCustomerIds();
        auto relateMeasure = [r](int i) -> double {
            return request_graph.adj_list[i][r];
        };
        sort(customer_list.begin(), customer_list.end(),
             [relateMeasure](const int & a, const int & b) -> bool
            { return relateMeasure(a) < relateMeasure(b); });
        y = (randomNumber() % 7000) / 10000.0;
        i = floor(pow(y, rho) * (int)customer_list.size());
        removed_customer.push_back(customer_list[i]);
        removeCustomer(s, customer_list[i]);
    }
}
void updateNeighborGraph(Solution s)
{
    if (!s.isFeasible()) return;
    double cost = s.getCost();
    for(Route r : s.getRoutes())
        for(int i = 0; i < (int)r.getCustomerId().size() - 1; i++){
            int u = r.getCustomerId()[i], v = r.getCustomerId()[i + 1];
            minimize(neighbor_graph.adj_list[u][v], cost);
            minimize(neighbor_graph.adj_list[v][u], cost);
        }
}
void updateRequestGraph(Solution s)
{
    if (!s.isFeasible()) return;

    for(Route r : s.getRoutes())
        for(int i = 0; i < (int)r.getCustomerId().size() - 1; i++)
            for(int j = i + 1; j < (int)r.getCustomerId().size() - 1; j++){
            int u = r.getCustomerId()[i], v = r.getCustomerId()[j];
            request_graph.adj_list[u][v]++;
            request_graph.adj_list[v][u]++;
        }
    request_graph_pool.push_back(s);
    if ((int)request_graph_pool.size() > Param::LNS::top_b){
        s = request_graph_pool[0];
        for(Route r : s.getRoutes())
            for(int i = 0; i < (int)r.getCustomerId().size() - 1; i++)
                for(int j = i + 1; j < (int)r.getCustomerId().size(); j++){
                int u = r.getCustomerId()[i], v = r.getCustomerId()[j];
                request_graph.adj_list[u][v]--;
                request_graph.adj_list[v][u]--;
                }
        request_graph_pool.erase(request_graph_pool.begin());
    }
}
void LNSMove::updateLNSMove(Solution s)
{
    updateNeighborGraph(s);
    updateRequestGraph(s);
}
void Insert::basicGreedyInsert(Solution& s)
{
    vector< int > removed_customer, request_bank, customer_list;


    bool stop = false;
    while(!stop){
        stop = true;
        int  f_route = 0, f_pos = 0;
        double f_dist = 2 * max_distance;
        request_bank = calculateRequestBank(s);
        removed_customer = request_bank;

        shuffle(removed_customer.begin(), removed_customer.end(),mt_rand);
        for(int customer : removed_customer){
            //Calculate f+(i,s)
            f_route = 0;
            f_pos = 0;
            f_dist = 2 * max_distance + 1;
            for(int i = 0; i < (int)s.getRoutes().size(); i++){
                Route r = s.getRoutes()[i];
                int r_cap = 0;
                bool had_linehaul = false;
                for(int j = 0; j < (int)r.getCustomerId().size(); j++){
                        if (instance.customers[r.getCustomerId()[j]].getType() == instance.customers[customer].getType())
                            r_cap += instance.customers[r.getCustomerId()[j]].getQuantity();
                        had_linehaul|= (instance.customers[r.getCustomerId()[j]].getType() == linehaul);
                    }
                if (r_cap + instance.customers[customer].getQuantity() > instance.cap) continue;
                if (had_linehaul == false && instance.customers[customer].getType() == backhaul)
                    continue;

                for(int j = 0; j < (int)r.getCustomerId().size() - 1; j++){
                    if (instance.customers[customer].getType() == linehaul
                        && instance.customers[r.getCustomerId()[j]].getType() == backhaul)
                        continue;
                    if (instance.customers[customer].getType() == backhaul
                        && instance.customers[r.getCustomerId()[j + 1]].getType() == linehaul)
                        continue;
                    double new_dist = getDist(r.getCustomerId()[j], customer)
                                    + getDist(r.getCustomerId()[j + 1], customer);
                    if (new_dist < f_dist){
                        f_dist = new_dist;
                        f_route = i;
                        f_pos = j;
                    }
                }
            }

            if (f_dist < 2 * max_distance){
                s.getRoutes()[f_route].getCustomerId().insert(s.getRoutes()[f_route].getCustomerId().begin() + f_pos + 1, customer);
                stop = false;
            }
        }

        if (!request_bank.empty() && stop == true){
            Route empty_route;
            empty_route.getCustomerId().push_back(instance.depot.getId());
            empty_route.getCustomerId().push_back(request_bank[0]);
            empty_route.getCustomerId().push_back(instance.depot.getId());
            s.getRoutes().push_back(empty_route);
            stop = false;
        }
    }
}
void Insert::deepGreedyInsert(Solution& s)
{
    vector< int > removed_customer, request_bank, customer_list;

    bool stop = false;

    while(!stop){
        stop = true;
        int f_customer = 0, f_route = 0, f_pos = 0;
        double f_dist = 2 * max_distance + 1;
        request_bank = calculateRequestBank(s);
        removed_customer = request_bank;
        //Find global best insertion
        for(int customer : removed_customer){
            //Calculate f+(i,s) for each customer
            int c_route = 0,
            c_pos = 0;
            double c_dist = 2 * max_distance + 1;

            for(int i = 0; i < (int)s.getRoutes().size(); i++){
                Route r = s.getRoutes()[i];
                int r_cap = 0;
                bool had_linehaul = false;
                for(int j = 0; j < (int)r.getCustomerId().size(); j++){
                    if (instance.customers[r.getCustomerId()[j]].getType() == instance.customers[customer].getType())
                        r_cap += instance.customers[r.getCustomerId()[j]].getQuantity();
                    had_linehaul|= (instance.customers[r.getCustomerId()[j]].getType() == linehaul);
                }
                if (r_cap + instance.customers[customer].getQuantity() > instance.cap) continue;
                if (had_linehaul == false && instance.customers[customer].getType() == backhaul)
                    continue;
                for(int j = 0; j < (int)r.getCustomerId().size() - 1; j++){
                    if (instance.customers[customer].getType() == linehaul
                        && instance.customers[r.getCustomerId()[j]].getType() == backhaul)
                        continue;
                    if (instance.customers[customer].getType() == backhaul
                        && instance.customers[r.getCustomerId()[j + 1]].getType() == linehaul)
                        continue;
                    double new_dist = getDist(r.getCustomerId()[j], customer)
                                    + getDist(r.getCustomerId()[j + 1], customer);
                    if (new_dist < c_dist){
                        c_dist = new_dist;
                        c_route = i;
                        c_pos = j;
                    }
                }
            }
            if (c_dist < f_dist){
                    f_dist = c_dist;
                    f_route = c_route;
                    f_pos = c_pos;
                    f_customer = customer;
            }
        }
        if (f_dist < 2 * max_distance){
            s.getRoutes()[f_route].getCustomerId().insert(s.getRoutes()[f_route].getCustomerId().begin() + f_pos + 1, f_customer);
            stop = false;
        }
        if (!request_bank.empty() && stop == true){
            Route empty_route;
            empty_route.getCustomerId().push_back(instance.depot.getId());
            empty_route.getCustomerId().push_back(request_bank[0]);
            empty_route.getCustomerId().push_back(instance.depot.getId());
            s.getRoutes().push_back(empty_route);
            stop = false;
        }
    }
    return;
}
void Insert::regretKInsert(Solution &s, int k)
{
    vector< int > removed_customer, request_bank, customer_list;
    bool stop = false;

    while(!stop){
        stop = true;
        int f_customer = 0, f_route = 0, f_pos = 0;
        double f_dist = -1;
        request_bank = calculateRequestBank(s);
        removed_customer = request_bank;
        //Find global best insertion
        for(int customer : removed_customer){
            //Calculate regret value for each customer
            int c_route = 0,
            c_pos = 0;
            double c_dist = 2 * max_distance + 1;

            vector< double > regret;

            for(int i = 0; i < (int)s.getRoutes().size(); i++){
                Route r = s.getRoutes()[i];
                bool had_linehaul = 0;
                int r_cap = 0;
                for(int j = 0; j < (int)r.getCustomerId().size(); j++){
                    if (instance.customers[r.getCustomerId()[j]].getType() == instance.customers[customer].getType())
                        r_cap += instance.customers[r.getCustomerId()[j]].getQuantity();
                    had_linehaul|= (instance.customers[r.getCustomerId()[j]].getType() == linehaul);
                }
                if (r_cap + instance.customers[customer].getQuantity() > instance.cap) continue;
                //cerr << i << ' ' << (instance.customers[customer].getType() == linehaul ? "line" : "back") << r_cap + instance.customers[customer].getQuantity() << '\n';
                if (had_linehaul == false && instance.customers[customer].getType() == backhaul)
                    continue;
                for(int j = 0; j < (int)r.getCustomerId().size() - 1; j++){
                    if (instance.customers[customer].getType() == linehaul
                        && instance.customers[r.getCustomerId()[j]].getType() == backhaul)
                        continue;
                    if (instance.customers[customer].getType() == backhaul
                        && instance.customers[r.getCustomerId()[j + 1]].getType() == linehaul)
                        continue;
                    double new_dist = getDist(r.getCustomerId()[j], customer)
                                    + getDist(r.getCustomerId()[j + 1], customer);
                    if (new_dist < c_dist){
                        c_dist = new_dist;
                        c_route = i;
                        c_pos = j;
                    }

                    if ((int)regret.size() < k)
                        regret.push_back(new_dist);
                    else if (new_dist < (*max_element(regret.begin(), regret.end()))){   //O(K), use with small K
                        *max_element(regret.begin(), regret.end()) = new_dist;
                    }
                }
            }
            sort(regret.begin(), regret.end());
            if (regret.empty())
                continue;
            double regret_value = 0;
            for(int i = 0; i < (int)regret.size(); i++)
                regret_value+= regret[i] - regret[0];

            //Save the argmax regret value
            if (regret_value > f_dist){
                    f_dist = regret_value;
                    f_route = c_route;
                    f_pos = c_pos;
                    f_customer = customer;
            }
        }

        if (f_dist >= 0){
            s.getRoutes()[f_route].getCustomerId().insert(s.getRoutes()[f_route].getCustomerId().begin() + f_pos + 1, f_customer);
            Route r = s.getRoutes()[f_route];
            stop = false;
        }
        if (!request_bank.empty() && stop == true){
            Route empty_route;
            empty_route.getCustomerId().push_back(instance.depot.getId());
            empty_route.getCustomerId().push_back(request_bank[0]);
            empty_route.getCustomerId().push_back(instance.depot.getId());
            s.getRoutes().push_back(empty_route);
            stop = false;
        }
    }
    return;
}
