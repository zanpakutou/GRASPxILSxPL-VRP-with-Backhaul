#include "Customer.h"

std::ostream& operator<<(std::ostream& out, const CustomerType value){
    const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(dehaul);
        PROCESS_VAL(linehaul);
        PROCESS_VAL(backhaul);
    }
#undef PROCESS_VAL

    return out << s;
}

Customer::Customer():
    type(linehaul)
    ,id(-1)
    ,x(-1)
    ,y(-1)
    ,quantity(-1)
{
}
Customer::Customer(int _id, int _x, int _y, int _quantity, CustomerType _type):
    type(_type)
    ,id(_id)
    ,x(_x)
    ,y(_y)
    ,quantity(_quantity)
{
}

int Customer::getId(){
    return this->id;
}
void Customer::setId(int id){
    this->id = id;
}
CustomerType Customer::getType(){
    return this->type;
}
void Customer::setType(CustomerType type){
    this->type = type;
}
int Customer::getX(){
    return this->x;
}
int Customer::getY(){
    return this->y;
}
int Customer::getQuantity(){
    return this->quantity;
}
