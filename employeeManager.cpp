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

StatusType EmployeeManager::AddCompany(int CompanyID, int Value)
{
   if (CompanyID<=0 || Value<=0) return INVALID_INPUT;
   std::shared_ptr<Company> company = companies->getValue(CompanyID);
   if (company != nullptr) return FAILURE;

   company = std::make_shared<Company>(Company(Value, CompanyID, nullptr,
   std::make_shared<AVLTree<EmployeeKey, Employee, std::weak_ptr>>(AVLTree<EmployeeKey, Employee, std::weak_ptr>()));
   if(company == nullptr) return ALLOCATION_ERROR;

   companies->add(CompanyID,company);
   return SUCCESS;
}

StatusType EmployeeManager::AddEmployee(int EmployeeID, int CompanyID, int Salary, int Grade)
{
   if(EmployeeID<=0 || CompanyID<=0 || Salary<=0 || Grade<=0) return INVALID_INPUT;
   std::weak_ptr<Company> company = companies->getValue(CompanyID);
   if(company == nullptr) return FAILURE;
   std::shared_ptr<Employee> employee = employees->getValue(EmployeeID);
   if (employee != nullptr) return FAILURE;
   employee = std::make_shared<Employee>(EmployeeID,Salary,Grade,company,this->dummy);

}