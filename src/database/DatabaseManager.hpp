#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <pqxx/pqxx>
#include "../models/Employee.hpp"
#include "../models/Competence.hpp"
#include "../models/Matrix.hpp"
#include "../models/Assessment.hpp"
#include "../models/Position.hpp"

namespace Database {

class DatabaseManager {
public:
    DatabaseManager(const std::string& connectionString);
    ~DatabaseManager();

    // Connection management
    bool connect();
    void disconnect();
    bool isConnected() const;

    // Employee operations
    int addEmployee(const Models::Employee& employee);
    Models::Employee getEmployee(int id);
    std::vector<Models::Employee> getAllEmployees();
    bool deleteEmployee(int id);

    // Competence operations
    int addCompetence(const Models::Competence& competence);
    std::vector<Models::Competence> getAllCompetences();
    bool deleteCompetence(int id);

    // Position operations
    int addPosition(const Models::Position& position);
    Models::Position getPosition(int id);
    std::vector<Models::Position> getAllPositions();
    bool updatePosition(const Models::Position& position);
    bool deletePosition(int id);
    std::vector<Models::Position> getPositionsByMatrix(int matrixId);

    // Matrix operations
    int addMatrix(const Models::Matrix& matrix);
    Models::Matrix getMatrix(int id);
    std::vector<Models::Matrix> getAllMatrices();
    bool deleteMatrix(int id);

    // Matrix-Competence relations
    bool addCompetencyToMatrix(int matrixId, int competenceId, int requiredLevel);
    bool removeCompetencyFromMatrix(int matrixId, int competenceId);
    std::vector<std::pair<int, int>> getMatrixCompetencies(int matrixId);

    // Assessment operations
    int addAssessment(const Models::Assessment& assessment);
    std::vector<Models::Assessment> getEmployeeAssessments(int employeeId);
    bool deleteAssessment(int id);

private:
    std::string connectionString;
    std::unique_ptr<pqxx::connection> connection;
    bool connected;

    std::unordered_map<std::string, std::string> queries;
    std::string queriesPath;

    void loadQueries(const std::string& path);
    const std::string& getQuery(const std::string& name) const;

    // Helper methods for parsing results
    Models::Employee parseEmployeeRow(const pqxx::row& row);
    Models::Competence parseCompetenceRow(const pqxx::row& row);
    Models::Position parsePositionRow(const pqxx::row& row);
    Models::Matrix parseMatrixRow(const pqxx::row& row);
    Models::Assessment parseAssessmentRow(const pqxx::row& row);
};

} // namespace Database
