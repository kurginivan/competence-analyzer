#pragma once

#include <string>
#include <ctime>

namespace Models {

class Position {
public:
    Position();
    Position(int id, const std::string& name, int matrixId = -1);

    // Getters
    int getId() const;
    const std::string& getName() const;
    int getMatrixId() const;
    std::time_t getCreatedAt() const;
    std::time_t getUpdatedAt() const;

    // Setters
    void setId(int id);
    void setName(const std::string& name);
    void setMatrixId(int matrixId);

    // Display
    void display() const;

private:
    int id;
    std::string name;
    int matrixId;
    std::time_t createdAt;
    std::time_t updatedAt;
};

} // namespace Models
