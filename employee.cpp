#include "employee.h"

EmployeeKey::EmployeeKey(int id, int salary):id(id),salary(salary){}

Employee::Employee(int id, int salary, int grade, std::weak_ptr<Company> employer, std::weak_ptr<Company> dummy):
id(id), salary(salary), grade(grade), employer(employer), dummy(dummy){}