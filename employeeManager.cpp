#include "library1.h"
#include "employeeManager.h"
#include "avlTree.h"
#include "company_employee.h"
#include <memory>
#include <cmath>

EmployeeManager::EmployeeManager(): 
   companies(std::unique_ptr<AVLTree<int,Company,std::shared_ptr>>(new AVLTree<int,Company,std::shared_ptr>())),
   employees(std::unique_ptr<AVLTree<int,Employee,std::shared_ptr>>(new AVLTree<int,Employee,std::shared_ptr>())),
   dummy(std::unique_ptr<Company>(new Company(0,0,std::weak_ptr<Employee>(),
   std::make_shared<AVLTree<EmployeeKey, Employee, std::weak_ptr>>(AVLTree<EmployeeKey, Employee, std::weak_ptr>()),
   std::make_shared<AVLTree<int, Employee, std::weak_ptr>>(AVLTree<int, Employee, std::weak_ptr>())))){}

StatusType EmployeeManager::AddCompany(int CompanyID, int Value)
{
   if (CompanyID<=0 || Value<=0) return INVALID_INPUT;
   std::shared_ptr<Company> company = companies->getValue(CompanyID).lock();
   if (company != nullptr) return FAILURE;

   company = std::make_shared<Company>(Company(Value, CompanyID, std::weak_ptr<Employee>(),
   std::make_shared<AVLTree<EmployeeKey, Employee, std::weak_ptr>>(AVLTree<EmployeeKey, Employee, std::weak_ptr>()),
   std::make_shared<AVLTree<int, Employee, std::weak_ptr>>(AVLTree<int, Employee, std::weak_ptr>())));
   if(company == nullptr) return ALLOCATION_ERROR;

   companies->add(CompanyID,company);//catch exception
   return SUCCESS;
}

StatusType EmployeeManager::AddEmployee(int EmployeeID, int CompanyID, int Salary, int Grade)
{
   if(EmployeeID<=0 || CompanyID<=0 || Salary<=0 || Grade<=0) return INVALID_INPUT;
   std::weak_ptr<Company> company = companies->getValue(CompanyID);
   if(company.lock() == nullptr) return FAILURE;
   std::shared_ptr<Employee> employee = employees->getValue(EmployeeID).lock();
   if (employee != nullptr) return FAILURE;
   employee = std::make_shared<Employee>(Employee(EmployeeID,Salary,Grade,company));
   if (employee==nullptr) return ALLOCATION_ERROR;
   employees->add(EmployeeID,employee); //add the employee to the general employee tree 
   //catch exception
   EmployeeKey current_key = EmployeeKey(EmployeeID,Salary);
   company.lock()->employees_by_salary->add(current_key,employee); //add the employee to its company
   //catch exception
   //check if we need to replace highest earner in company
   if ( company.lock()->highest_earner.lock() == nullptr ) 
   {
      company.lock()->highest_earner = employee;
   }
   else
   {
      EmployeeKey old_highest_key = EmployeeKey(company.lock()->highest_earner.lock()->id, company.lock()->highest_earner.lock()->salary);
      if( old_highest_key < current_key ) 
      {
         company.lock()->highest_earner = employee;
      }
   }
   dummy->employees_by_salary->add(current_key,employee); //add the employee to dummy
   //catch exception
   //check if the highest earner in dummy needs to be replaced
   if ( dummy->highest_earner.lock() == nullptr ) 
   {
      dummy->highest_earner = employee;
   }
   else
   {
      EmployeeKey old_highest_key_dummy = EmployeeKey(dummy->highest_earner.lock()->id, dummy->highest_earner.lock()->salary);
      if( old_highest_key_dummy < current_key ) 
      {
         dummy->highest_earner = employee;
      }
   }
   return SUCCESS;
}

StatusType EmployeeManager::RemoveCompany(int CompanyID)
{
   if ( companies <= 0 ) return INVALID_INPUT;
   std::weak_ptr<Company> company = companies->getValue(CompanyID);
   if(company.lock() == nullptr) return FAILURE;
   if( !company.lock()->employees_by_salary->isEmpty()) return FAILURE;
   companies->remove(CompanyID); //catch exception
   return SUCCESS;
}

StatusType EmployeeManager::RemoveEmployee(int EmployeeID)
{
   if ( EmployeeID <= 0 ) return INVALID_INPUT;
   std::shared_ptr<Employee> employee = std::make_shared<Employee>(employees->getValue(EmployeeID));
   if( employee == nullptr ) return FAILURE;
   EmployeeKey key = EmployeeKey(employee->id, employee->salary);
   employee->employer.lock()->employees_by_salary->remove(key); //catch exception
   //find the new max!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   dummy->employees_by_salary->remove(key); //catch exception
   //find the new max!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   employees->remove(EmployeeID); //catch exception
   return SUCCESS;
}

StatusType EmployeeManager::GetCompanyInfo(int CompanyID, int *Value, int *NumEmployees)
{
   if ( CompanyID<=0 || Value==nullptr || NumEmployees==nullptr ) return INVALID_INPUT;
   std::weak_ptr<Company> company = companies->getValue(CompanyID);
   if( company.lock() == nullptr ) return FAILURE;
   *Value = company.lock()->getValue();
   //*NumEmployees = company.lock()->employees->size(); //////////////////add size function to the tree!!!!
   return SUCCESS;
}

StatusType EmployeeManager::GetEmployeeInfo(int EmployeeID, int *EmployerID, int *Salary, int *Grade)
{
   if (EmployeeID<=0 || EmployerID==nullptr || Salary==nullptr || Grade==nullptr) return INVALID_INPUT;
   std::weak_ptr<Employee> employee = employees->getValue(EmployeeID);
   if( employee.lock() == nullptr ) return FAILURE;
   *Salary = employee.lock()->salary;
   *Grade = employee.lock()->grade;
   *EmployerID = employee.lock()->employer.lock()->id;
   return SUCCESS;
}

StatusType EmployeeManager::IncreaseCompanyValue(int CompanyID, int ValueIncrease)
{
   if ( CompanyID <= 0 || ValueIncrease <= 0 ) return INVALID_INPUT;
   std::weak_ptr<Company> company = companies->getValue(CompanyID);
   if( company.lock() == nullptr ) return FAILURE;
   company.lock()->increaseValue(ValueIncrease); //catch exception
   return SUCCESS;
}

StatusType EmployeeManager::PromoteEmployee(int EmployeeID, int SalaryIncrease, int BumpGrade)
{
   if (EmployeeID <= 0 || SalaryIncrease <= 0 ) return INVALID_INPUT;
   std::weak_ptr<Employee> employee = employees->getValue(EmployeeID);
   if( employee.lock() == nullptr ) return FAILURE;
   int new_salary = employee.lock()->salary + SalaryIncrease;
   int new_grade = employee.lock()->grade;
   if(BumpGrade>0) new_grade++;
   int company_id = employee.lock()->employer.lock()->id;
   StatusType res1 = RemoveEmployee(EmployeeID);
   if (res1 == ALLOCATION_ERROR) return ALLOCATION_ERROR;
   StatusType res2 = AddEmployee(EmployeeID, company_id, new_salary, new_grade);
   if (res2 == ALLOCATION_ERROR) return ALLOCATION_ERROR;
   return SUCCESS;
}

StatusType EmployeeManager::HireEmployee(int EmployeeID, int NewCompanyID)
{
   if ( EmployeeID<=0 || NewCompanyID<=0 ) return INVALID_INPUT;
   std::weak_ptr<Employee> employee = employees->getValue(EmployeeID);
   if( employee.lock() == nullptr ) return FAILURE;
   std::weak_ptr<Company> new_company = companies->getValue(NewCompanyID);
   if( new_company.lock() == nullptr ) return FAILURE;
   EmployeeKey key = EmployeeKey(EmployeeID,employee.lock()->salary);
   //check if the employee already works at the new company:
   if(new_company.lock()->employees_by_salary->getValue(key).lock() != nullptr) return FAILURE;
   //remove employee from old company:
   employee.lock()->employer.lock()->employees_by_salary->remove(key);
   //employee.lock()->employer.lock()->highest_earner = new max!!!!!
   employee.lock()->employer = new_company;
   new_company.lock()->employees_by_salary->add(key,employee.lock());
   //new_company.lock()->highest_earner = new maxxxxx!!!!!!!!!
   return SUCCESS;
}

StatusType EmployeeManager::AcquireCompany(int AcquirerID, int TargetID, double Factor)
{
   if ( AcquirerID <= 0 || TargetID <= 0 || Factor < 1.00 || AcquirerID == TargetID ) return INVALID_INPUT;
   std::weak_ptr<Company> acquirer = companies->getValue(AcquirerID);
   if( acquirer.lock() == nullptr ) return FAILURE;
   std::weak_ptr<Company> target = companies->getValue(TargetID);
   if( target.lock() == nullptr ) return FAILURE;
   if (acquirer.lock()->getValue() < 10*(target.lock()->getValue())) return FAILURE;
   //move the employees of target to the acquirer:
   acquirer.lock()->employees_by_salary->addAVLTree(target.lock()->employees_by_salary); //catch exception
   //delete all employees from target so you can delete the company
   /////////////target.lock()->employees->CLEAR THE TREE
   acquirer.lock()->increaseValue(target.lock()->getValue());
   double addition_factor = Factor - 1;
   int addition = (int)floor(addition_factor * acquirer.lock()->getValue());
   acquirer.lock()->increaseValue(addition);
   StatusType res = RemoveCompany(TargetID);
   return res; //should always return success
}

StatusType EmployeeManager::GetHighestEarner(int CompanyID, int *EmployeeID)
{
   if ( CompanyID == 0 || EmployeeID == nullptr) return INVALID_INPUT;
   if (CompanyID < 0)
   {
      std::weak_ptr<Company> company = companies->getValue(CompanyID);
      if ( company.lock() == nullptr ) return FAILURE;
      //first check if there are employees in this company
      if ( company.lock()->employees_by_salary->isEmpty() ) return FAILURE;
      *EmployeeID = company.lock()->highest_earner.lock()->id;
      return SUCCESS;
   }
   //we get here only if CompanyID < 0 !!
   //if there are no employees in the system:
   if (dummy->employees_by_salary->isEmpty()) return FAILURE;
   *EmployeeID = dummy->highest_earner.lock()->id;
   return SUCCESS;
}



















