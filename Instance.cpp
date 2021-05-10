#include "Instance.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
Instance instance;


Instance::Instance(){};
Instance::Instance(std::string dir, std::string instanceSet , std::string instanceName ){
    if (instanceSet == "GBJ"){
        string dir = "instance/CASE";
        //cerr << dir + instanceName[0] + ".CUS" << ' ' << dir + instanceName + ".VRB" << '\n';
        ifstream CUS(dir + instanceName[0] + ".CUS");
        ifstream DEP(dir + instanceName[0] + ".DEP");
        ifstream SUP(dir + instanceName[0] + ".SUP");

        ifstream VRB(dir + instanceName + ".VRB");

        cerr << "Reading " << instanceName << "... \n";
        int nb_supplier;
        string line, temp, filename;
        while(getline(VRB, line)){
            stringstream ss(line);
            if (line.find("[number_of_customers]") != string::npos)
                ss >> temp >> this->nb_customer;
            if (line.find("[number_of_routes]") != string::npos)
                ss >> temp >> this->nb_route;
            if (line.find("[number_of_suppliers]") != string::npos)
                ss >> temp >> nb_supplier;
            if (line.find("[vehicle_capacity]") != string::npos)
                ss >> temp >> this->cap;
        }

        int id, x_coordinate, y_coordinate, command;

        //Read depot
        getline(DEP, line);
        stringstream ss(line);
        ss >> temp >> x_coordinate >> y_coordinate >> command;
        this->depot = Customer(0, x_coordinate, y_coordinate, command, dehaul);
        this->customers.push_back(depot);

        //Read customers
        for(int i = 0; i < this->nb_customer; i++){
            getline(CUS, line);
            stringstream ss(line);
            ss >> id >> x_coordinate >> y_coordinate >> command;
            this->customers.push_back(Customer(id, x_coordinate, y_coordinate, command, linehaul));
        }

        //Read suppliers
        for(int i = 0; i < nb_supplier; i++){
            getline(SUP, line);
            stringstream ss(line);
            ss >> id >> x_coordinate >> y_coordinate >> command;
            this->customers.push_back(Customer(id + this->nb_customer, x_coordinate, y_coordinate, command, backhaul));
        }
        this->nb_customer += nb_supplier + 1;
        cerr << "Reading done.\n";
    }
};
Customer Instance::getCustomer(Route a, int id)
{
    return instance.customers[a.getCustomerId()[id]];
}
