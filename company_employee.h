#ifndef COMPANY_EMPLOYEE_H_
#define COMPANY_EMPLOYEE_H_
#include <memory>
#include "avlTree.h"
#include <iostream>

class EmployeeKey
{
    public:
    int id;
    int salary;
    EmployeeKey(int id, int salary);
    EmployeeKey();
    EmployeeKey(const EmployeeKey& emp) = default;
    EmployeeKey& operator=(const EmployeeKey& emp) = default;
    bool operator<(const EmployeeKey& other_key);
    bool operator==(const EmployeeKey& other_key);
    bool operator!=(const EmployeeKey& other_key);
    friend std::ostream& operator<<(std::ostream& os, const EmployeeKey& key);
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
    Employee(const Employee& emp) = default;
    Employee& operator=(const Employee& emp) = default;
    void promote(int salary_increase, bool bump_grade);
};

class Company
{
    private:
    int value;

    public:
    int id;
    std::weak_ptr<Employee> highest_earner;
    std::shared_ptr<AVLTree<EmployeeKey, Employee, std::shared_ptr>> employees_by_salary;
    std::shared_ptr<AVLTree<int, Employee, std::shared_ptr>> employees_by_id;

    Company(int value, int id, std::weak_ptr<Employee> highest_earner,
            std::shared_ptr<AVLTree<EmployeeKey, Employee, std::shared_ptr>> employees,
            std::shared_ptr<AVLTree<int, Employee, std::shared_ptr>> employees_by_id);
    Company(const Company& comp) = default;
    Company& operator=(const Company& comp) = default;

    int getValue();
    void increaseValue(int amount);
};

#endif //COMPANY_EMPLOYEE_H_