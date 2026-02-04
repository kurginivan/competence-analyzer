#pragma once

#include <string>
#include <ctime>

namespace Models {

class Employee {
public:
    Employee();
    Employee(int id, const std::string& name, const std::string& position,
             const std::string& department, const std::string& email);

    // Getters
    int getId() const;
    const std::string& getName() const;
    const std::string& getPosition() const;
    const std::string& getDepartment() const;
    const std::string& getEmail() const;
    std::time_t getCreatedAt() const;
    std::time_t getUpdatedAt() const;

    // Setters
    void setId(int id);
    void setName(const std::string& name);
    void setPosition(const std::string& position);
    void setDepartment(const std::string& department);
    void setEmail(const std::string& email);

    // Display
    void display() const;

private:
    int id;
    std::string name;
    std::string position;
    std::string department;
    std::string email;
    std::time_t createdAt;
    std::time_t updatedAt;
};

} // namespace Models
