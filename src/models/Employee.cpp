#include "Employee.hpp"
#include <iostream>
#include <ctime>

namespace Models {

Employee::Employee()
  : id(0), name(""), positionId(-1), department(""), email(""),
      createdAt(std::time(nullptr)), updatedAt(std::time(nullptr)) {}

Employee::Employee(int id, const std::string& name, int positionId,
                   const std::string& department, const std::string& email)
  : id(id), name(name), positionId(positionId), department(department), email(email),
      createdAt(std::time(nullptr)), updatedAt(std::time(nullptr)) {}

int Employee::getId() const { return id; }
const std::string& Employee::getName() const { return name; }
int Employee::getPositionId() const { return positionId; }
const std::string& Employee::getDepartment() const { return department; }
const std::string& Employee::getEmail() const { return email; }
std::time_t Employee::getCreatedAt() const { return createdAt; }
std::time_t Employee::getUpdatedAt() const { return updatedAt; }

void Employee::setId(int id) { this->id = id; }
void Employee::setName(const std::string& name) { this->name = name; }
void Employee::setPositionId(int positionId) { this->positionId = positionId; }
void Employee::setDepartment(const std::string& department) { this->department = department; }
void Employee::setEmail(const std::string& email) { this->email = email; }

void Employee::display() const {
    std::cout << "ID: " << id << "\n"
              << "Имя: " << name << "\n"
              << "ID должности: " << (positionId == -1 ? 0 : positionId) << "\n"
              << "Отдел: " << department << "\n"
              << "Email: " << email << "\n";
}

} // namespace Models
