#include "library1.h"
#include "employeeManager.h"
#include "avlTree.h"
#include "employee.h"
#include "company.h"
#include <memory>

EmployeeManager::EmployeeManager(): 
   companies(std::unique_ptr<AVLTree<int,Company,std::shared_ptr>>(new AVLTree<int,Company,std::shared_ptr>())),
   employees(std::unique_ptr<AVLTree<int,Employee,std::shared_ptr>>(new AVLTree<int,Employee,std::shared_ptr>())),
   dummy(std::unique_ptr<Company>(new Company(0,0,nullptr,
   std::make_shared<AVLTree<EmployeeKey, Employee, std::weak_ptr>>(AVLTree<EmployeeKey, Employee, std::weak_ptr>()
   )))){}

void *Init()
{
   EmployeeManager manager();
}

StatusType AddCompany(void *DS, int CompanyID, int Value)
{
   
}