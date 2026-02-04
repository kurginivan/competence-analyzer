#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <map>

namespace Models {

class Matrix {
public:
    Matrix();
    Matrix(int id, const std::string& name, const std::string& description);

    // Getters
    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    std::time_t getCreatedAt() const;
    std::time_t getUpdatedAt() const;

    // Setters
    void setId(int id);
    void setName(const std::string& name);
    void setDescription(const std::string& description);

    // Competency requirements management
    void addCompetencyRequirement(int competenceId, int requiredLevel);
    void removeCompetencyRequirement(int competenceId);
    int getRequiredLevel(int competenceId) const;
    const std::map<int, int>& getCompetencyRequirements() const;

    // Display
    void display() const;

private:
    int id;
    std::string name;
    std::string description;
    std::time_t createdAt;
    std::time_t updatedAt;
    std::map<int, int> competencyRequirements; // competenceId -> requiredLevel
};

} // namespace Models
