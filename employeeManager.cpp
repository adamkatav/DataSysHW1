#include "library1.h"
#include "employeeManager.h"
#include "avlTree.h"
#include "company_employee.h"
#include <memory>
#include <cmath>


EmployeeManager::EmployeeManager(): 
   empty_companies(std::unique_ptr<AVLTree<int,Company,std::shared_ptr>>(new AVLTree<int,Company,std::shared_ptr>())),
   non_empty_companies(std::unique_ptr<AVLTree<int,Company,std::shared_ptr>>(new AVLTree<int,Company,std::shared_ptr>())),
   dummy(std::unique_ptr<Company>(new Company(0,0,std::weak_ptr<Employee>(),
   std::make_shared<AVLTree<EmployeeKey, Employee, std::shared_ptr>>(AVLTree<EmployeeKey, Employee, std::shared_ptr>()),
   std::make_shared<AVLTree<int, Employee, std::shared_ptr>>(AVLTree<int, Employee, std::shared_ptr>())))){}

StatusType EmployeeManager::AddCompany(int CompanyID, int Value)
{
   if (CompanyID<=0 || Value<=0)
      return INVALID_INPUT;
   std::shared_ptr<Company> company = empty_companies->getValue(CompanyID).lock();
   if (company != nullptr)
      company = non_empty_companies->getValue(CompanyID).lock();
   if (company != nullptr)
      return FAILURE;
   company = std::make_shared<Company>(Company(Value, CompanyID, std::weak_ptr<Employee>(),
   std::make_shared<AVLTree<EmployeeKey, Employee, std::shared_ptr>>(AVLTree<EmployeeKey, Employee, std::shared_ptr>()),
   std::make_shared<AVLTree<int, Employee, std::shared_ptr>>(AVLTree<int, Employee, std::shared_ptr>())));
   if(company == nullptr)
      return ALLOCATION_ERROR;
   //at first the company will not have any employees so we add it to the empty companies tree
   empty_companies->add(CompanyID,company);
   return SUCCESS;
}

StatusType EmployeeManager::AddEmployee(int EmployeeID, int CompanyID, int Salary, int Grade)
{
   auto EMPTY_COMP = empty_companies.get();
   auto NON_EMPTY_COMP = non_empty_companies.get();
   if(EmployeeID<=0 || CompanyID<=0 || Salary<=0 || Grade<0)
      return INVALID_INPUT;
   std::weak_ptr<Company> company = empty_companies->getValue(CompanyID);
   if(company.lock() == nullptr)
      company = non_empty_companies->getValue(CompanyID);
   if(company.lock() == nullptr)
      return FAILURE;
   std::shared_ptr<Employee> employee = dummy->employees_by_id->getValue(EmployeeID).lock();
   if (employee != nullptr)
      return FAILURE;

   if(company.lock() == nullptr)
      throw std::runtime_error("employer was freed in AddEmployee");

   employee = std::make_shared<Employee>(Employee(EmployeeID,Salary,Grade,company.lock()));
   if (employee==nullptr)
      return ALLOCATION_ERROR;
   EmployeeKey current_key = EmployeeKey(EmployeeID,Salary);

   dummy->employees_by_id->add(EmployeeID,employee);
   dummy->employees_by_salary->add(current_key,employee); 
   //check if the highest earner in dummy needs to be replaced
   dummy->highest_earner = dummy->employees_by_salary->getMax();
   company.lock()->employees_by_salary->add(current_key,employee);
   company.lock()->employees_by_id->add(EmployeeID,employee);
   if(company.lock()->employees_by_salary->isEmpty())
      throw std::runtime_error("employees_by_salary is empty in AddingEmployee");
   company.lock()->highest_earner = company.lock()->employees_by_salary->getMax(); //update the highest earner
   
   //check if this is the company's only worker
   if( company.lock()->employees_by_id->size==1 )
   {
      non_empty_companies->add(CompanyID,company.lock());
      empty_companies->remove(CompanyID);
   }
   return SUCCESS;
}

StatusType EmployeeManager::RemoveCompany(int CompanyID)
{
   if ( CompanyID <= 0 )
      return INVALID_INPUT;
   std::weak_ptr<Company> company = empty_companies->getValue(CompanyID);
   if(company.lock() == nullptr)
      return FAILURE;
   if(!company.lock()->employees_by_salary->isEmpty())
      throw std::runtime_error("Company is empty but has employees");
   empty_companies->remove(CompanyID); 
   return SUCCESS;
}

StatusType EmployeeManager::RemoveEmployee(int EmployeeID)
{
   if ( EmployeeID <= 0 )
      return INVALID_INPUT;
   std::shared_ptr<Employee> employee = dummy->employees_by_id->getValue(EmployeeID).lock();
   if( employee == nullptr )
      return FAILURE;
   EmployeeKey key = EmployeeKey(employee->id, employee->salary);
   auto Employer = employee->employer.lock();
   dummy->employees_by_id->remove(EmployeeID); 
   dummy->employees_by_salary->remove(key); 
   if(!dummy->employees_by_salary->isEmpty())
      dummy->highest_earner = dummy->employees_by_salary->getMax();
   else
      dummy->highest_earner = std::shared_ptr<Employee>();
   Employer->employees_by_id->remove(EmployeeID); 
   Employer->employees_by_salary->remove(key); 
   if(!Employer->employees_by_salary->isEmpty())
      Employer->highest_earner = Employer->employees_by_salary->getMax();
   else{
      empty_companies->add(Employer->id, Employer);
      non_empty_companies->remove(Employer->id);
      Employer->highest_earner = std::shared_ptr<Employee>();
   }
   return SUCCESS;
}

StatusType EmployeeManager::GetCompanyInfo(int CompanyID, int *Value, int *NumEmployees)
{
   if ( CompanyID<=0 || Value==nullptr || NumEmployees==nullptr )
      return INVALID_INPUT;
   std::weak_ptr<Company> company = empty_companies->getValue(CompanyID);
   if( company.lock() == nullptr )
   {
      company = non_empty_companies->getValue(CompanyID);
      if (company.lock() == nullptr)
         return FAILURE;
   }
   *Value = company.lock()->getValue();
   *NumEmployees = company.lock()->employees_by_id->size;
   return SUCCESS;
}

StatusType EmployeeManager::GetEmployeeInfo(int EmployeeID, int *EmployerID, int *Salary, int *Grade)
{
   auto EMPTY_COMP = empty_companies.get();
   auto NON_EMPTY_COMP = non_empty_companies.get();
   if (EmployeeID<=0 || EmployerID==nullptr || Salary==nullptr || Grade==nullptr)
      return INVALID_INPUT;
   std::weak_ptr<Employee> employee = dummy->employees_by_id->getValue(EmployeeID);
   if( employee.lock() == nullptr )
      return FAILURE;
   *Salary = employee.lock()->salary;
   *Grade = employee.lock()->grade;
   auto REMOVE_ON_SIGHT = employee.lock()->employer.lock();
   *EmployerID = REMOVE_ON_SIGHT->id;
   return SUCCESS;
}

StatusType EmployeeManager::IncreaseCompanyValue(int CompanyID, int ValueIncrease)
{
   if ( CompanyID <= 0 || ValueIncrease <= 0 )
      return INVALID_INPUT;
   std::weak_ptr<Company> company = empty_companies->getValue(CompanyID);
   if( company.lock() == nullptr )
   {
      company = non_empty_companies->getValue(CompanyID);
      if ( company.lock() == nullptr )
         return FAILURE;
   }
   company.lock()->increaseValue(ValueIncrease); //catch exception
   return SUCCESS;
}

StatusType EmployeeManager::PromoteEmployee(int EmployeeID, int SalaryIncrease, int BumpGrade)
{
   if (EmployeeID <= 0 || SalaryIncrease <= 0 )
      return INVALID_INPUT;
   std::shared_ptr<Employee> employee = dummy->employees_by_id->getValue(EmployeeID).lock();
   if( employee == nullptr )
      return FAILURE;
   int new_salary = employee->salary + SalaryIncrease;
   int new_grade = employee->grade;
   if(BumpGrade>0)
      new_grade++;
   int company_id = employee->employer.lock()->id;
   StatusType res1 = RemoveEmployee(EmployeeID);
   if (res1 == ALLOCATION_ERROR || res1==FAILURE)
      return res1;
   StatusType res2 = AddEmployee(EmployeeID, company_id, new_salary, new_grade);
   if (res2 == ALLOCATION_ERROR)
      return ALLOCATION_ERROR;
   return SUCCESS;
}

StatusType EmployeeManager::HireEmployee(int EmployeeID, int NewCompanyID)
{
   if ( EmployeeID<=0 || NewCompanyID<=0 )
      return INVALID_INPUT;
   std::shared_ptr<Employee> employee = dummy->employees_by_id->getValue(EmployeeID).lock();
   if( employee == nullptr )
      return FAILURE;
   int salary = employee->salary;
   int grade = employee->grade;
   std::shared_ptr<Company> new_company = empty_companies->getValue(NewCompanyID).lock();
   if( new_company == nullptr )
   {
      new_company = non_empty_companies->getValue(NewCompanyID).lock();
      if ( new_company == nullptr )
         return FAILURE;
   }
   EmployeeKey key = EmployeeKey(EmployeeID,employee->salary);
   //check if the employee already works at the new company:
   if(new_company->employees_by_salary->getValue(key).lock() != nullptr)
      return FAILURE;
   // //remove employee from old company:
   // employee->employer.lock()->employees_by_salary->remove(key); //catch exception
   // employee->employer.lock()->employees_by_id->remove(EmployeeID); //catch exception
   // employee->employer.lock()->highest_earner = employee->employer.lock()->employees_by_salary->getMax();
   // if(employee->employer.lock()->employees_by_id->isEmpty()){
   //    empty_companies->add(employee->employer.lock()->id, employee->employer.lock());
   //    non_empty_companies->remove(employee->employer.lock()->id);
   // }
   // if(new_company == nullptr)
   //    throw std::runtime_error("employer was freed in HireEmployee");
   // employee->employer = new_company;
   // if(new_company->employees_by_id->isEmpty()){
   //    non_empty_companies->add(new_company->id, new_company);
   //    empty_companies->remove(new_company->id);
   // }
   // new_company->employees_by_salary->add(key,employee); //catch exception
   // new_company->employees_by_id->add(EmployeeID,employee); //catch exception
   // if(new_company->employees_by_salary->isEmpty())
   //    throw std::runtime_error("new_company->employees_by_salary is empty in HireEmployee");
   
   // new_company->highest_earner =  new_company->employees_by_salary->getMax();
   
   StatusType result1 = RemoveEmployee(EmployeeID);
   if(result1 == FAILURE)
      return FAILURE;
   StatusType result2 = AddEmployee(EmployeeID, NewCompanyID, salary, grade);
   
   return result2;
}

StatusType EmployeeManager::AcquireCompany(int AcquirerID, int TargetID, double Factor)
{
   if ( AcquirerID <= 0 || TargetID <= 0 || Factor < 1.00 || AcquirerID == TargetID )
      return INVALID_INPUT;
   std::weak_ptr<Company> acquirer = empty_companies->getValue(AcquirerID);
   if( acquirer.lock() == nullptr )
   {
      acquirer = non_empty_companies->getValue(AcquirerID);
      if( acquirer.lock() == nullptr )
         return FAILURE;
   }
   std::weak_ptr<Company> target = empty_companies->getValue(TargetID);
   if( target.lock() == nullptr )
   {
      target = non_empty_companies->getValue(TargetID);
      if( target.lock() == nullptr )
         return FAILURE;
   }
   if (acquirer.lock()->getValue() < 10*(target.lock()->getValue()))
      return FAILURE;
   auto min_employee_id = target.lock()->employees_by_id->getMin()->id;
   auto max_employee_id = target.lock()->employees_by_id->getMax()->id;
   auto num_of_employees_in_target = target.lock()->employees_by_id->size;
   auto target_employees_array = target.lock()->employees_by_id->flattenvaluesArray(num_of_employees_in_target, min_employee_id, max_employee_id);

   for (int i = 0; i < num_of_employees_in_target; i++){
      target_employees_array[i]->employer = acquirer;
   }
   


   //move the employees of target to the acquirer:
   acquirer.lock()->employees_by_salary->addAVLTree(target.lock()->employees_by_salary); //catch exception
   acquirer.lock()->employees_by_id->addAVLTree(target.lock()->employees_by_id); //catch exception
   target.lock()->employees_by_salary->clear();
   target.lock()->employees_by_id->clear();
   acquirer.lock()->increaseValue(target.lock()->getValue());
   double addition_factor = Factor - 1;
   int addition = (int)floor(addition_factor * acquirer.lock()->getValue());
   acquirer.lock()->increaseValue(addition);
   StatusType res = RemoveCompany(TargetID);
   return res; //should always return success
}

StatusType EmployeeManager::GetHighestEarner(int CompanyID, int *EmployeeID)
{
   if ( CompanyID == 0 || EmployeeID == nullptr)
      return INVALID_INPUT;
   if (CompanyID > 0)
   {
      std::weak_ptr<Company> company = empty_companies->getValue(CompanyID);
      if ( company.lock() == nullptr )
      {
         company = non_empty_companies->getValue(CompanyID);
         if ( company.lock() == nullptr )
            return FAILURE;
         *EmployeeID = company.lock()->highest_earner.lock()->id;
         return SUCCESS;
      }
      return FAILURE; //if we get here then the company is empty of workers
   }
   //we get here only if CompanyID < 0 !!
   //if there are no employees in the system:
   if (dummy->employees_by_salary->isEmpty())
      return FAILURE;
   *EmployeeID = dummy->highest_earner.lock()->id;
   return SUCCESS;
}

StatusType EmployeeManager::GetAllEmployeesBySalary(int CompanyID, int **Employees, int *NumOfEmployees)
{
   if ( CompanyID==0 || Employees==nullptr || NumOfEmployees==nullptr )
      return INVALID_INPUT;
   int size = 0;
   std::shared_ptr<EmployeeKey[]> arr_ascending = nullptr;
   if ( CompanyID > 0)
   {
      auto company = empty_companies->getValue(CompanyID);
      if ( company.lock() != nullptr)
         return FAILURE; //this means that the company has no employees
      company = non_empty_companies->getValue(CompanyID);
      if ( company.lock() == nullptr )
         return FAILURE; //this means that the company doesn't exist
      size = company.lock()->employees_by_salary->size;
      *NumOfEmployees = size;
      //*Employees = new int[size];
      *Employees = (int*)malloc(size*sizeof(int));
      if (*Employees == nullptr )
         return ALLOCATION_ERROR;
      arr_ascending = company.lock()->employees_by_salary->flattenKeysArray();
      for (int i=0 ; i<size ; i++)
      {
         (*Employees)[i] = arr_ascending[size-1-i].id;
      }
      return SUCCESS;
   }
   //if companyID < 0
   size = dummy->employees_by_id->size;
   *NumOfEmployees = size;
   // *Employees = new int[size];
   *Employees = (int*)malloc(size*sizeof(int));
   if (*Employees == nullptr )
      return ALLOCATION_ERROR;
   arr_ascending = dummy->employees_by_salary->flattenKeysArray();
   for (int i=0 ; i<size ; i++)
   {
      (*Employees)[i] = arr_ascending[size-1-i].id;
   }
   return SUCCESS;
}

StatusType EmployeeManager::GetHighestEarnerInEachCompany(int NumOfCompanies, int **Employees)
{
   if ( Employees==nullptr || NumOfCompanies<1 )
      return INVALID_INPUT;
   if ( non_empty_companies->size < NumOfCompanies )
      return FAILURE;
   int min_company = non_empty_companies->getMin()->id;
   int max_company = non_empty_companies->getMax()->id;
   auto company_values_arr = non_empty_companies->flattenvaluesArray(NumOfCompanies,min_company,max_company);
   //*Employees = new int[NumOfCompanies];
   *Employees = (int*)malloc(NumOfCompanies*sizeof(int));
   if (*Employees == nullptr )
      return ALLOCATION_ERROR;
   for (int i = 0 ; i < NumOfCompanies ; i++)
   {
      (*Employees)[i] = company_values_arr[i]->highest_earner.lock()->id;
   }
   return SUCCESS;
}

StatusType EmployeeManager::GetNumEmployeesMatching(int CompanyID, int MinEmployeeID, int MaxEmployeeId,
            int MinSalary, int MinGrade, int *TotalNumOfEmployees, int *NumOfEmployees)
{
   if ( CompanyID==0 || MinEmployeeID<0 || MaxEmployeeId<0 || MinSalary<0 || MinGrade<0 || TotalNumOfEmployees==nullptr
      || NumOfEmployees==nullptr || MinEmployeeID>MaxEmployeeId)
         return INVALID_INPUT;

   if ( CompanyID > 0 )
   {
      auto company = empty_companies->getValue(CompanyID);
      if ( company.lock() != nullptr )
         return FAILURE;
      company = non_empty_companies->getValue(CompanyID);
      if ( company.lock() == nullptr )
         return FAILURE;
      auto employee_arr = company.lock()->employees_by_id->flattenvaluesArray(company.lock()->employees_by_id->size, MinEmployeeID, MaxEmployeeId);
      int i = 0;
      *TotalNumOfEmployees = 0;
      *NumOfEmployees = 0;
      while (employee_arr[i]->id <= MaxEmployeeId)
      {
         (*TotalNumOfEmployees)++;
         if ( employee_arr[i]->salary>=MinSalary && employee_arr[i]->grade>=MinGrade)
         {
            (*NumOfEmployees)++;
         }
         i++;
      }
      return SUCCESS;
   }
   if (dummy->employees_by_id->isEmpty())
      return FAILURE;
   auto employee_arr_total = dummy->employees_by_id->flattenvaluesArray(dummy->employees_by_id->size, MinEmployeeID, MaxEmployeeId);
      int j = 0;
      *TotalNumOfEmployees = 0;
      *NumOfEmployees = 0;
      while (employee_arr_total[j]->id <= MaxEmployeeId)
      {
         (*TotalNumOfEmployees)++;
         if ( employee_arr_total[j]->salary>=MinSalary && employee_arr_total[j]->grade>=MinGrade)
         {
            (*NumOfEmployees)++;
         }
         j++;
      }
      return SUCCESS;
}

