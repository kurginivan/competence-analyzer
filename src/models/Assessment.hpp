#pragma once

#include <string>
#include <ctime>

namespace Models {

class Assessment {
public:
    Assessment();
    Assessment(int id, int employeeId, int competenceId, int actualLevel,
               const std::string& comments = "");

    // Getters
    int getId() const;
    int getEmployeeId() const;
    int getCompetenceId() const;
    int getActualLevel() const;
    const std::string& getComments() const;
    std::time_t getAssessmentDate() const;
    std::time_t getCreatedAt() const;
    std::time_t getUpdatedAt() const;

    // Setters
    void setId(int id);
    void setEmployeeId(int employeeId);
    void setCompetenceId(int competenceId);
    void setActualLevel(int actualLevel);
    void setComments(const std::string& comments);

    // Validation
    bool isValid() const;

    // Display
    void display() const;

private:
    int id;
    int employeeId;
    int competenceId;
    int actualLevel;
    std::string comments;
    std::time_t assessmentDate;
    std::time_t createdAt;
    std::time_t updatedAt;
};

} // namespace Models
