#include "Employee.hpp"
#include <iostream>
#include <ctime>

namespace Models {

Employee::Employee()
    : id(0), name(""), position(""), department(""), email(""),
      createdAt(std::time(nullptr)), updatedAt(std::time(nullptr)) {}

Employee::Employee(int id, const std::string& name, const std::string& position,
                   const std::string& department, const std::string& email)
    : id(id), name(name), position(position), department(department), email(email),
      createdAt(std::time(nullptr)), updatedAt(std::time(nullptr)) {}

int Employee::getId() const { return id; }
const std::string& Employee::getName() const { return name; }
const std::string& Employee::getPosition() const { return position; }
const std::string& Employee::getDepartment() const { return department; }
const std::string& Employee::getEmail() const { return email; }
std::time_t Employee::getCreatedAt() const { return createdAt; }
std::time_t Employee::getUpdatedAt() const { return updatedAt; }

void Employee::setId(int id) { this->id = id; }
void Employee::setName(const std::string& name) { this->name = name; }
void Employee::setPosition(const std::string& position) { this->position = position; }
void Employee::setDepartment(const std::string& department) { this->department = department; }
void Employee::setEmail(const std::string& email) { this->email = email; }

void Employee::display() const {
    std::cout << "ID: " << id << "\n"
              << "Имя: " << name << "\n"
              << "Должность: " << position << "\n"
              << "Отдел: " << department << "\n"
              << "Email: " << email << "\n";
}

} // namespace Models
