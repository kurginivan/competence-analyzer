#include "Competence.hpp"
#include <iostream>
#include <ctime>

namespace Models {

Competence::Competence()
    : id(0), name(""), description(""), category(""),
      createdAt(std::time(nullptr)), updatedAt(std::time(nullptr)) {}

Competence::Competence(int id, const std::string& name, const std::string& description,
                       const std::string& category)
    : id(id), name(name), description(description), category(category),
      createdAt(std::time(nullptr)), updatedAt(std::time(nullptr)) {}

int Competence::getId() const { return id; }
const std::string& Competence::getName() const { return name; }
const std::string& Competence::getDescription() const { return description; }
const std::string& Competence::getCategory() const { return category; }
std::time_t Competence::getCreatedAt() const { return createdAt; }
std::time_t Competence::getUpdatedAt() const { return updatedAt; }

void Competence::setId(int id) { this->id = id; }
void Competence::setName(const std::string& name) { this->name = name; }
void Competence::setDescription(const std::string& description) { this->description = description; }
void Competence::setCategory(const std::string& category) { this->category = category; }

void Competence::display() const {
    std::cout << "ID: " << id << "\n"
              << "Название: " << name << "\n"
              << "Категория: " << category << "\n"
              << "Описание: " << description << "\n";
}

} // namespace Models
