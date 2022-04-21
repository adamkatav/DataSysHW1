#include "company.h"
#include <memory>

Company::Company(int value, int id, std::shared_ptr<AVLTree<EmployeeKey, Employee, std::weak_ptr>> employees, 
            std::weak_ptr<Employee> highest_earner):value(value), id(id), employees(employees), 
            highest_earner(highest_earner) {}

int Company::getValue()
{
    return value;
}

void Company::increaseValue(int amount)
{
    value += amount;
}