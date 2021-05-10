#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <iostream>

enum CustomerType{
    linehaul, backhaul, dehaul
} ;
std::ostream& operator<<(std::ostream& out, const CustomerType value);

class Customer{
private:
    CustomerType type;
    int id;
    int x, y, quantity;
public:
    int getId();
    void setId(int id);
    CustomerType getType();
    void setType(CustomerType type);
    int getX();
    int getY();
    int getQuantity();
    Customer();
    Customer(int _id, int _x, int _y, int _quantity, CustomerType _type);
};

#endif // CUSTOMER_H
