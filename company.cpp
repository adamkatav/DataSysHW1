#include "company.h"
#include "employee.h"
#include "avlTree.h"
#include <memory>

Company::Company(int value, int id, std::weak_ptr<Employee> highest_earner,
                std::shared_ptr<AVLTree<EmployeeKey, Employee, std::weak_ptr>> employees): 
                value(value), id(id), highest_earner(highest_earner),employees(employees){}

int Company::getValue()
{
    return value;
}

void Company::increaseValue(int amount)
{
    value += amount;
}