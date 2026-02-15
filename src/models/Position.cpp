#include "Position.hpp"
#include <iostream>

namespace Models {

Position::Position() 
    : id(0), name(""), matrixId(-1), createdAt(0), updatedAt(0) {}

Position::Position(int id, const std::string& name, int matrixId)
    : id(id), name(name), matrixId(matrixId), createdAt(0), updatedAt(0) {}

int Position::getId() const { return id; }
const std::string& Position::getName() const { return name; }
int Position::getMatrixId() const { return matrixId; }
std::time_t Position::getCreatedAt() const { return createdAt; }
std::time_t Position::getUpdatedAt() const { return updatedAt; }

void Position::setId(int id) { this->id = id; }
void Position::setName(const std::string& name) { this->name = name; }
void Position::setMatrixId(int matrixId) { this->matrixId = matrixId; }

void Position::display() const {
    std::cout << "ID: " << id << "\n";
    std::cout << "Название: " << name << "\n";
    std::cout << "ID матрицы: " << (matrixId != -1 ? std::to_string(matrixId) : "не указана") << "\n";
}

} // namespace Models
