#pragma once

#include <string>
#include <ctime>

namespace Models {

class Competence {
public:
    Competence();
    Competence(int id, const std::string& name, const std::string& description,
               const std::string& category);

    // Getters
    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::string& getCategory() const;
    std::time_t getCreatedAt() const;
    std::time_t getUpdatedAt() const;

    // Setters
    void setId(int id);
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setCategory(const std::string& category);

    // Display
    void display() const;

private:
    int id;
    std::string name;
    std::string description;
    std::string category;
    std::time_t createdAt;
    std::time_t updatedAt;
};

} // namespace Models
