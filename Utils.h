#pragma once

#include <random>
#include <functional>
#include <iostream>
#include "Param.h"
#include "Instance.h"

inline std::mt19937::result_type seed = Param::seed;
inline std::mt19937 mt_rand(seed);
inline auto randomCustomer = std::bind(std::uniform_int_distribution< int >(0, instance.nb_customer),
                                                                  std::mt19937(seed));
inline auto randomNumber = std::bind(std::uniform_int_distribution< int >(0, INT_MAX),
                           std::mt19937(seed));
inline bool minimize(double& x, double value){
    if (x > value) {x = value; return true;}
    return false;
}
inline double getDist(int i, int j){
    int x1 = instance.customers[i].getX();
    int y1 = instance.customers[i].getY();
    int x2 = instance.customers[j].getX();
    int y2 = instance.customers[j].getY();
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1- y2));
}
//#endif // UTILS_H
