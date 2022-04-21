#include "employee.h"
#include "avlTree.h"
#include <memory>

EmployeeKey::EmployeeKey(int id, int salary):id(id),salary(salary){}

bool EmployeeKey::operator<(EmployeeKey other_key)
{
    if (salary < other_key.salary) 
    {
        return true;
    }
    else if (salary == other_key.salary && id > other_key.id) 
    {
        return true;
    }
    return false;
}

Employee::Employee(int id, int salary, int grade, std::weak_ptr<Company> employer,
        std::weak_ptr<Company> dummy): id(id), salary(salary), grade(grade), 
        employer(employer), dummy(dummy){}

void Employee::promote(int salary_increase, bool bump_grade)
{
    salary += salary_increase;
    if(bump_grade)
    {
        grade ++;
    }
}