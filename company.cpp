#include "company_employee.h"
#include "avlTree.h"
#include <memory>

Company::Company(int value, int id, std::weak_ptr<Employee> highest_earner,
                std::shared_ptr<AVLTree<EmployeeKey, Employee, std::weak_ptr>> employees_by_salary,
                std::shared_ptr<AVLTree<int, Employee, std::weak_ptr>> employees_by_id): 
                value(value), id(id), highest_earner(highest_earner),employees_by_salary(employees_by_salary),
                employees_by_id(employees_by_id){}

Company::Company():value(-1),id(-1),highest_earner(std::make_shared<Employee>(Employee())),
                    employees_by_salary(std::make_shared<AVLTree<EmployeeKey, Employee, std::weak_ptr>>(AVLTree<EmployeeKey, Employee, std::weak_ptr>())),
                    employees_by_id(std::make_shared<AVLTree<int, Employee, std::weak_ptr>>(AVLTree<int, Employee, std::weak_ptr>())){}

int Company::getValue()
{
    return value;
}

void Company::increaseValue(int amount)
{
    value += amount;
}