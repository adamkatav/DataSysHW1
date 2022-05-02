#include "company_employee.h"
#include "avlTree.h"
#include <memory>

EmployeeKey::EmployeeKey(int id, int salary):id(id),salary(salary){}
//Adam
EmployeeKey::EmployeeKey(){}

std::ostream& operator<<(std::ostream& os, const EmployeeKey& key)
{
    os << key.id;
    return os;
}

bool EmployeeKey::operator<(const EmployeeKey& other_key)
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

bool EmployeeKey::operator==(const EmployeeKey& other_key)
{
    return (id == other_key.id);
}

bool EmployeeKey::operator!=(const EmployeeKey& other_key)
{
    return (id != other_key.id);
}

Employee::Employee(int id, int salary, int grade, std::weak_ptr<Company> employer): 
                    id(id), salary(salary), grade(grade), employer(employer){}

void Employee::promote(int salary_increase, bool bump_grade)
{
    salary += salary_increase;
    if(bump_grade)
    {
        grade ++;
    }
}