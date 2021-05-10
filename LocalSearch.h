#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "Instance.h"
#include "Solution.h"
#include "Route.h"

///TODO : fix local search
//Inter-route
/*
---------   ---------       -----\  /---\  /-----\
                                  \/     \/
       2k opt*           ->       /\     /\
---------   ---------       -----/  \---/  \-----/
*/
bool twokoptstarMove(Route &a, Route &b);
/*
------------------       -----\  /\  /-----
                               \/  \/
       swap          ->        /\  /\
------------------       -----/  \/  \-----
*/
bool swapMove(Route &a, Route &b);
/*
------------------       --------/\-------
                                /  \
       relocate      ->        /    \
------------------       -----/      \-----
*/
bool relocateMove(Route &a, Route &b);
//Intra-route
bool TSP_optimizer(Route& a);
void LocalSearch(Solution& s);

#endif // LOCALSEARCH_H
