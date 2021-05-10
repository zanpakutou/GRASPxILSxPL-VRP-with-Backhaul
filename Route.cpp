#include "Route.h"
#include "Instance.h"
#include "Customer.h"
#include "Utils.h"

using namespace std;

vector< int >& Route::getCustomerId(){
    return this->customer_id;
}

void Route::setCustomerId(vector< int >& v){
    this->customer_id = v;
}

/*
@return cost of route
*/

double Route::getCost(){
    double c = 0;
    for(int i = 0; i < (int)this->getCustomerId().size() - 1; i++){
        c+= getDist(this->getCustomerId()[i], this->getCustomerId()[i + 1]);
    }
    return c;
}


/*
@return true if route is feasible
        false otherwise
*/

bool Route::isFeasible(){
    if (this->getCustomerId()[0] != instance.depot.getId() || this->getCustomerId().back() != instance.depot.getId()){
       //cerr << "infeasible : lack of depot\n";
        return false;
    }
    bool has_linehauls = false, had_backhaul = false;
    for(int i = 1; i < (int)this->getCustomerId().size(); i++){
        if (instance.customers[this->getCustomerId()[i]].getType() == linehaul){
            has_linehauls = true;
            if (had_backhaul) {
                //cerr << "infeasible: backhaul before linehaul\n";
                return false;
            }
        }
        if (instance.customers[this->getCustomerId()[i]].getType() == backhaul){
            had_backhaul = true;
        }
    }
    if (!has_linehauls) {
            //cerr << "infeasible: no linehaul\n";
            return false;
    }

    int cap_linehaul = 0, cap_backhaul = 0;

    for(int i = 0; i < (int)this->getCustomerId().size(); i++){
        Customer current_customer = instance.customers[this->getCustomerId()[i]];
        if (current_customer.getType() == linehaul)
                cap_linehaul += current_customer.getQuantity();
        if (current_customer.getType() == backhaul)
            cap_backhaul += current_customer.getQuantity();
    }
    if (cap_linehaul > instance.cap || cap_backhaul > instance.cap){
        //cerr << "infeasible: Capacity exceed\n";
        return false;
    }

    return true;
}

