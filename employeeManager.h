#ifndef EMPLOYEE_MANAGER_H_
#define EMPLOYEE_MANAGER_H_
#include <memory>
#include "avlTree.h"
#include "employee.h"
#include "company.h"

class EmployeeManager
{
    public:
    std::unique_ptr<AVLTree<int, Company, std::shared_ptr>> companies;
    std::unique_ptr<AVLTree<int, Employee, std::shared_ptr>> employees;
    std::shared_ptr<Company> dummy;

    EmployeeManager();
};


#endif //EMPLOYEE_MANAGER_H_