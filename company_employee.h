#ifndef COMPANY_EMPLOYEE_H_
#define COMPANY_EMPLOYEE_H_
#include <memory>
#include "avlTree.h"

class EmployeeKey
{
    public:
    const int id;
    int salary;
    EmployeeKey(int id, int salary);
    bool operator<(const EmployeeKey& other_key);
    bool operator==(const EmployeeKey& other_key);
};

class Company;

class Employee
{
    public:
    const int id;
    int salary;
    int grade;
    std::weak_ptr<Company> employer;

    Employee(int id, int salary, int grade, std::weak_ptr<Company> employer);
    void promote(int salary_increase, bool bump_grade);
};

class Company
{
    private:
    int value;

    public:
    const int id;
    std::weak_ptr<Employee> highest_earner;
    std::shared_ptr<AVLTree<EmployeeKey, Employee, std::weak_ptr>> employees_by_salary;
    std::shared_ptr<AVLTree<int, Employee, std::weak_ptr>> employees_by_id;

    Company(int value, int id, std::weak_ptr<Employee> highest_earner,
            std::shared_ptr<AVLTree<EmployeeKey, Employee, std::weak_ptr>> employees,
            std::shared_ptr<AVLTree<int, Employee, std::weak_ptr>> employees_by_id);
    int getValue();
    void increaseValue(int amount);
};

#endif //COMPANY_EMPLOYEE_H_