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
   if(Employer->employees_by_id->getValue(EmployeeID).lock()==nullptr)
      throw std::runtime_error("employee not in employer in RemoveEmployee");
   Employer->employees_by_id->remove(EmployeeID); 
   Employer->employees_by_salary->remove(key); 
   if(!Employer->employees_by_salary->isEmpty())
      Employer->highest_earner = Employer->employees_by_salary->getMax();
   else{
      empty_companies->add(Employer->id, Employer);
      non_empty_companies->remove(Employer->id);
      Employer->highest_earner = std::shared_ptr<Employee>();
   }
   dummy->employees_by_id->remove(EmployeeID); 
   dummy->employees_by_salary->remove(key); 
   if(!dummy->employees_by_salary->isEmpty())
      dummy->highest_earner = dummy->employees_by_salary->getMax();
   else
      dummy->highest_earner = std::shared_ptr<Employee>();
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
   int old_salary = employee->salary;
   int new_salary = old_salary  + SalaryIncrease;
   int new_grade = employee->grade;
   if(BumpGrade>0)
      new_grade++;
   auto company = employee->employer.lock();
   int company_id = company->id;
   company->employees_by_salary->remove(EmployeeKey(EmployeeID,old_salary));
   auto new_employee = std::make_shared<Employee>(Employee(EmployeeID,new_salary,new_grade,company));
   if (employee==nullptr)
      return ALLOCATION_ERROR;
   company->employees_by_salary->add(EmployeeKey(EmployeeID,new_salary),new_employee);
   company->employees_by_id->remove(EmployeeID);
   company->employees_by_id->add(EmployeeID,new_employee);
   company->highest_earner = company->employees_by_salary->getMax();
   dummy->employees_by_salary->remove(EmployeeKey(EmployeeID,old_salary));
   dummy->employees_by_salary->add(EmployeeKey(EmployeeID,new_salary),new_employee);
   dummy->employees_by_id->remove(EmployeeID);
   dummy->employees_by_id->add(EmployeeID,new_employee);
   dummy->highest_earner = dummy->employees_by_salary->getMax();
   // StatusType res1 = RemoveEmployee(EmployeeID);
   // if (res1 == ALLOCATION_ERROR || res1==FAILURE)
   //    return res1;
   // StatusType res2 = AddEmployee(EmployeeID, company_id, new_salary, new_grade);
   // if (res2 == ALLOCATION_ERROR|| res2==FAILURE)
   //    return res2;
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
   EmployeeKey key = EmployeeKey(EmployeeID,salary);
   //check if the employee already works at the new company:
   if(new_company->employees_by_id->getValue(EmployeeID).lock() != nullptr)
      return FAILURE;

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
   auto acquirer = empty_companies->getValue(AcquirerID).lock();
   if( acquirer == nullptr )
   {
      acquirer = non_empty_companies->getValue(AcquirerID).lock();
      if( acquirer == nullptr )
         return FAILURE;
   }
   auto target = empty_companies->getValue(TargetID).lock();
   if( target == nullptr )
   {
      target = non_empty_companies->getValue(TargetID).lock();
      if( target == nullptr )
         return FAILURE;
   }
   int target_value = target->getValue();
   if (acquirer->getValue() < 10*(target_value))
      return FAILURE;

   if( target->employees_by_id->size==0)
   {
      empty_companies->remove(TargetID);
      return SUCCESS;
   }
   // auto min_employee_id = target->employees_by_id->getMin()->id;
   // auto max_employee_id = target->employees_by_id->getMax()->id;
   // auto num_of_employees_in_target = target->employees_by_id->size;
   // auto target_employees_array = target->employees_by_id->flattenvaluesArray(num_of_employees_in_target, min_employee_id, max_employee_id);

   // for (int i = 0; i < num_of_employees_in_target; i++){
   //    target_employees_array[i]->employer = acquirer;
   // }

   //move the employees of target to the acquirer:
   acquirer->employees_by_salary->addAVLTree(target->employees_by_salary);
   acquirer->employees_by_id->addAVLTree(target->employees_by_id); 
   target->employees_by_salary->clear();
   target->employees_by_id->clear();
   if(non_empty_companies->getValue(TargetID).lock() != nullptr)
      non_empty_companies->remove(TargetID);
   else //the target is in empty companies- this isn't possible but we'll check it anyway
      empty_companies->remove(TargetID);

   //for each moved employee, change the employer to the acquring company:
   EmployeeKey min_key = EmployeeKey(acquirer->employees_by_salary->getMin()->id, acquirer->employees_by_salary->getMin()->salary);
   EmployeeKey max_key = EmployeeKey(acquirer->employees_by_salary->getMax()->id, acquirer->employees_by_salary->getMax()->salary);
   int min_id = acquirer->employees_by_id->getMin()->id;
   int max_id = acquirer->employees_by_id->getMax()->id;
   int num_of_employees = acquirer->employees_by_salary->size;
   auto salary_values = acquirer->employees_by_salary->flattenvaluesArray(num_of_employees, min_key, max_key);
   auto id_values = acquirer->employees_by_id->flattenvaluesArray(num_of_employees,min_id,max_id);
   for(int i = 0 ; i < num_of_employees ; i++)
   {
      salary_values[i]->employer = acquirer;
      id_values[i]->employer = acquirer;
   }
   acquirer->increaseValue(target_value);
   double addition_factor = Factor - 1;
   int addition = (int)floor(addition_factor * acquirer->getValue());
   acquirer->increaseValue(addition);
   return SUCCESS; //should always return success
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
      int counter = company.lock()->employees_by_id->size;
      auto employee_arr = company.lock()->employees_by_id->flattenvaluesArray(counter, MinEmployeeID, MaxEmployeeId);
      int i = 0;
      *TotalNumOfEmployees = 0;
      *NumOfEmployees = 0;
      while (employee_arr[i] != nullptr)
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

