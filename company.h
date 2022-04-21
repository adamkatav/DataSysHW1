#ifndef COMPANY_H_
#define COMPANY_H_
#include <memory>
#include "employee.h"
#include "avlTree.h"

class Company
{
    private:
    int value;

    public:
    const int id;
    std::shared_ptr<AVLTree<EmployeeKey, Employee, std::weak_ptr>> employees;
    std::weak_ptr<Employee> highest_earner;

    Company(int value, int id, std::shared_ptr<AVLTree<EmployeeKey, Employee, std::weak_ptr>> employees, 
            std::weak_ptr<Employee> highest_earner);
    int getValue();
    void increaseValue(int amount);
    
};


#endif //COMPANY_H_