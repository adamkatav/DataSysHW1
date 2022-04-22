#ifndef EMPLOYEE_H_
#define EMPLOYEE_H_
#include <memory>
#include "company.h"
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

#endif //EMPLOYRR_H_