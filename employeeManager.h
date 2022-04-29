#ifndef EMPLOYEE_MANAGER_H_
#define EMPLOYEE_MANAGER_H_
#include <memory>
#include "avlTree.h"
#include "company_employee.h"
#include "library1.h"

class EmployeeManager
{
    public:
    std::unique_ptr<AVLTree<int, Company, std::shared_ptr>> companies;
    std::unique_ptr<AVLTree<int, Employee, std::shared_ptr>> employees;
    std::shared_ptr<Company> dummy;
    EmployeeManager();
    StatusType AddCompany(int CompanyID, int Value);
    StatusType AddEmployee(int EmployeeID, int CompanyID, int Salary, int Grade);
    StatusType RemoveCompany(int CompanyID);
    StatusType RemoveEmployee(int EmployeeID);
    StatusType GetCompanyInfo(int CompanyID, int *Value, int *NumEmployees);
    StatusType GetEmployeeInfo(int EmployeeID, int *EmployerID, int *Salary, int *Grade);
    StatusType IncreaseCompanyValue(int CompanyID, int ValueIncrease);
    StatusType PromoteEmployee(int EmployeeID, int SalaryIncrease, int BumpGrade);
    StatusType HireEmployee(int EmployeeID, int NewCompanyID);
    StatusType AcquireCompany(int AcquirerID, int TargetID, double Factor);
    StatusType GetHighestEarner(int CompanyID, int *EmployeeID);
    StatusType GetAllEmployeesBySalary(int CompanyID, int **Employees, int *NumOfEmployees);
    StatusType GetHighestEarnerInEachCompany(int NumOfCompanies, int **Employees);
    StatusType GetNumEmployeesMatching(int CompanyID, int MinEmployeeID, int MaxEmployeeId,
            int MinSalary, int MinGrade, int *TotalNumOfEmployees, int *NumOfEmployees);
};


#endif //EMPLOYEE_MANAGER_H_