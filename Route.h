#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <math.h>

using namespace std;

class Route {
private:
    vector< int > customer_id;
public:
    vector< int >& getCustomerId();
    void setCustomerId(vector< int >& v);
    double getCost();
    bool isFeasible();
};

#endif // ROUTE_H
