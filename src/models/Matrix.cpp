#include "Matrix.hpp"
#include <iostream>
#include <ctime>

namespace Models {

Matrix::Matrix()
    : id(0), name(""), description(""),
      createdAt(std::time(nullptr)), updatedAt(std::time(nullptr)) {}

Matrix::Matrix(int id, const std::string& name, const std::string& description)
    : id(id), name(name), description(description),
      createdAt(std::time(nullptr)), updatedAt(std::time(nullptr)) {}

int Matrix::getId() const { return id; }
const std::string& Matrix::getName() const { return name; }
const std::string& Matrix::getDescription() const { return description; }
std::time_t Matrix::getCreatedAt() const { return createdAt; }
std::time_t Matrix::getUpdatedAt() const { return updatedAt; }

void Matrix::setId(int id) { this->id = id; }
void Matrix::setName(const std::string& name) { this->name = name; }
void Matrix::setDescription(const std::string& description) { this->description = description; }

void Matrix::addCompetencyRequirement(int competenceId, int requiredLevel) {
    if (requiredLevel >= 1 && requiredLevel <= 5) {
        competencyRequirements[competenceId] = requiredLevel;
    }
}

void Matrix::removeCompetencyRequirement(int competenceId) {
    competencyRequirements.erase(competenceId);
}

int Matrix::getRequiredLevel(int competenceId) const {
    auto it = competencyRequirements.find(competenceId);
    if (it != competencyRequirements.end()) {
        return it->second;
    }
    return 0;
}

const std::map<int, int>& Matrix::getCompetencyRequirements() const {
    return competencyRequirements;
}

void Matrix::display() const {
    std::cout << "ID: " << id << "\n"
              << "Название: " << name << "\n"
              << "Описание: " << description << "\n"
              << "Компетенции: " << competencyRequirements.size() << "\n";
}

} // namespace Models
