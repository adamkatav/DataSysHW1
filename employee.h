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
    bool operator<(EmployeeKey other_key);
    bool operator==(EmployeeKey other_key);
};

class Employee
{
    public:
    const int id;
    int salary;
    int grade;
    std::weak_ptr<Company> employer;
    std::weak_ptr<Company> dummy;

    Employee(int id, int salary, int grade, std::weak_ptr<Company> employer, 
            std::weak_ptr<Company> dummy);
    void promote(int salary_increase, bool bump_grade);
};

#endif //EMPLOYRR_H_