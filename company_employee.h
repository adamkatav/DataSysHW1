#ifndef COMPANY_EMPLOYEE_H_
#define COMPANY_EMPLOYEE_H_
#include <memory>
#include "avlTree.h"

class EmployeeKey
{
    public:
    int id;
    int salary;
    EmployeeKey(int id, int salary);
    EmployeeKey();
    EmployeeKey& operator=(const EmployeeKey& other_key) = default;
    ~EmployeeKey() = default;
    bool operator<(const EmployeeKey& other_key);
    bool operator==(const EmployeeKey& other_key);
    bool operator!=(const EmployeeKey& other_key);
};

class Company;

class Employee
{
    public:
    int id;
    int salary;
    int grade;
    std::weak_ptr<Company> employer;

    Employee(int id, int salary, int grade, std::weak_ptr<Company> employer);
    Employee();
    Employee& operator=(const Employee& other_employee) = default;
    ~Employee() = default;
    void promote(int salary_increase, bool bump_grade);
};

class Company
{
    private:
    int value;

    public:
    int id;
    std::weak_ptr<Employee> highest_earner;
    std::shared_ptr<AVLTree<EmployeeKey, Employee, std::weak_ptr>> employees_by_salary;
    std::shared_ptr<AVLTree<int, Employee, std::weak_ptr>> employees_by_id;

    Company(int value, int id, std::weak_ptr<Employee> highest_earner,
            std::shared_ptr<AVLTree<EmployeeKey, Employee, std::weak_ptr>> employees,
            std::shared_ptr<AVLTree<int, Employee, std::weak_ptr>> employees_by_id);
    Company();
    Company& operator=(const Company& other_company) = default;
    ~Company() = default;
    int getValue();
    void increaseValue(int amount);
};

#endif //COMPANY_EMPLOYEE_H_