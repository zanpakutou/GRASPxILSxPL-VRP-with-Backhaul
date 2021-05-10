#pragma once
#include "Customer.h"
#include "Route.h"
#include <vector>
#include <string>
#include <fstream>


struct Instance {
    Customer depot;
    int nb_customer, nb_route, cap;
    std::vector< Customer > customers;

    Instance();
    Instance(std::string dir, std::string instanceSet, std::string instanceName);

    Customer getCustomer(Route a, int id);
};

extern Instance instance;
