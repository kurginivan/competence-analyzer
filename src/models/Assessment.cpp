#include "Assessment.hpp"
#include <iostream>
#include <ctime>

namespace Models {

Assessment::Assessment()
    : id(0), employeeId(0), competenceId(0), actualLevel(0), comments(""),
      assessmentDate(std::time(nullptr)), createdAt(std::time(nullptr)),
      updatedAt(std::time(nullptr)) {}

Assessment::Assessment(int id, int employeeId, int competenceId, int actualLevel,
                       const std::string& comments)
    : id(id), employeeId(employeeId), competenceId(competenceId), 
      actualLevel(actualLevel), comments(comments),
      assessmentDate(std::time(nullptr)), createdAt(std::time(nullptr)),
      updatedAt(std::time(nullptr)) {}

int Assessment::getId() const { return id; }
int Assessment::getEmployeeId() const { return employeeId; }
int Assessment::getCompetenceId() const { return competenceId; }
int Assessment::getActualLevel() const { return actualLevel; }
const std::string& Assessment::getComments() const { return comments; }
std::time_t Assessment::getAssessmentDate() const { return assessmentDate; }
std::time_t Assessment::getCreatedAt() const { return createdAt; }
std::time_t Assessment::getUpdatedAt() const { return updatedAt; }

void Assessment::setId(int id) { this->id = id; }
void Assessment::setEmployeeId(int employeeId) { this->employeeId = employeeId; }
void Assessment::setCompetenceId(int competenceId) { this->competenceId = competenceId; }
void Assessment::setActualLevel(int actualLevel) { 
    if (actualLevel >= 1 && actualLevel <= 5) {
        this->actualLevel = actualLevel;
    }
}
void Assessment::setComments(const std::string& comments) { this->comments = comments; }

bool Assessment::isValid() const {
    return employeeId > 0 && competenceId > 0 && actualLevel >= 1 && actualLevel <= 5;
}

void Assessment::display() const {
    std::cout << "ID: " << id << "\n"
              << "ID сотрудника: " << employeeId << "\n"
              << "ID компетенции: " << competenceId << "\n"
              << "Фактический уровень: " << actualLevel << "/5\n"
              << "Комментарии: " << comments << "\n";
}

} // namespace Models
