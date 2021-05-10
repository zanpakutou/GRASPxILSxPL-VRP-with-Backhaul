#include "Solution.h"
#include "Route.h"
#include "Instance.h"

vector< Route >& Solution::getRoutes(){
    return this->routes;
}

vector< int > Solution::getCustomerIds(){
    int _size = 0;
    for(Route r : this->getRoutes())
        _size+= (int)r.getCustomerId().size() - 2;
    vector< int > result(_size);
    int len = 0;
    for(Route r : this->getRoutes())
        for(int c :  r.getCustomerId())
            if (c != instance.depot.getId())
                result[len] = c, len++;
    return result;
}

void Solution::setRoutes(vector< Route > &v){
    this->routes = v;
}


double Solution::getCost(){
    double ans = 0;
    for(int i = 0; i < (int)this->getRoutes().size(); i++){
        ans += this->getRoutes()[i].getCost();
    }
    return ans;
}

bool Solution::isFeasible(){
    for(int i = 0 ; i < (int)this->getRoutes().size();i++)
        if (!this->getRoutes()[i].isFeasible()){
            //cerr << "Route constraint fail\n";
            return false;
        }

    vector< bool > used(instance.nb_customer, false);

    for(Route route : this->getRoutes())
        for(int customer : route.getCustomerId()){
                if (customer == instance.depot.getId()) continue;
                if (used[customer] == true){
                    //cerr << "infeasible :  duplicate customer\n";
                    return false;
                }
                used[customer] = true;
        }
    for(int i = 0; i < instance.nb_customer; i++){
        if (i == instance.depot.getId())
            continue;
        if (used[i] == false) {
                //cerr << "infeasible : customer not used\n";
                return false;
        }
    }
    if ((int)this->getRoutes().size() != instance.nb_route)
        return false;
    return true;
}
void Solution::print(ostream& of)
{
    of << this->isFeasible() << '\n';
    of << fixed << this->getCost() << '\n';
    for(int i = 0; i < (int)this->getRoutes().size(); i++){
        of << "Route " << i << " : ";
        for(auto customer : this->getRoutes()[i].getCustomerId())
            of << customer << ' ';
        of << '\n';
    }
}
