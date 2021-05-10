#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <iostream>
#include "Route.h"

using namespace std;

class Solution {
    private:
    vector< Route > routes;

    public:
    vector< Route >& getRoutes();
    vector< int > getCustomerIds();
    void setRoutes(vector< Route > &v);
    double getCost();
    bool isFeasible();
    void print(ostream& of);
} ;

#endif // SOLUTION_H
