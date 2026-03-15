#include "DatabaseManager.hpp"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cstdlib>

namespace {

std::string trimCopy(const std::string& value) {
    const char* whitespace = " \t\r\n";
    const auto start = value.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    const auto end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}

}

namespace Database {

DatabaseManager::DatabaseManager(const std::string& connectionString)
    : connectionString(connectionString), connected(false) {
    const char* envPath = std::getenv("COMP_ANALYZER_CPP_SQL");
    queriesPath = envPath ? envPath : "sql/queries_cpp.sql";
    try {
        loadQueries(queriesPath);
    } catch (const std::exception&) {
        if (!envPath) {
            queriesPath = "../sql/queries_cpp.sql";
            loadQueries(queriesPath);
        } else {
            throw;
        }
    }
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::connect() {
    try {
        connection = std::make_unique<pqxx::connection>(connectionString);
        if (connection->is_open()) {
            connected = true;
            std::cout << "Database connected successfully\n";
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка подключения к базе данных: " << e.what() << "\n";
        connected = false;
    }
    return false;
}

void DatabaseManager::disconnect() {
    if (connection && connection->is_open()) {
        connection.reset();
        connected = false;
    }
}

bool DatabaseManager::isConnected() const {
    return connected && connection && connection->is_open();
}

void DatabaseManager::loadQueries(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open SQL queries file: " + path);
    }

    std::string line;
    std::string currentName;
    std::ostringstream buffer;

    auto flush = [&]() {
        if (!currentName.empty()) {
            queries[currentName] = buffer.str();
            buffer.str("");
            buffer.clear();
        }
    };

    while (std::getline(file, line)) {
        if (line.rfind("-- name:", 0) == 0) {
            flush();
            currentName = trimCopy(line.substr(8));
            continue;
        }
        if (currentName.empty()) {
            continue;
        }
        buffer << line << "\n";
    }

    flush();

    if (queries.empty()) {
        throw std::runtime_error("No SQL queries loaded from: " + path);
    }
}

const std::string& DatabaseManager::getQuery(const std::string& name) const {
    const auto it = queries.find(name);
    if (it == queries.end()) {
        throw std::runtime_error("SQL query not found: " + name);
    }
    return it->second;
}

// Employee operations
int DatabaseManager::addEmployee(const Models::Employee& employee) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        pqxx::result result;
        if (employee.getPositionId() != -1) {
            result = txn.exec_params(
                getQuery("employees_insert_with_position"),
                employee.getName(),
                employee.getPositionId(),
                employee.getDepartment(),
                employee.getEmail()
            );
        } else {
            result = txn.exec_params(
                getQuery("employees_insert_without_position"),
                employee.getName(),
                employee.getDepartment(),
                employee.getEmail()
            );
        }
        txn.commit();
        return result[0]["id"].as<int>();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при добавлении сотрудника: " << e.what() << "\n";
        throw;
    }
}

Models::Employee DatabaseManager::getEmployee(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(getQuery("employees_get_by_id"), id);
        if (result.empty()) {
            throw std::runtime_error("Сотрудник не найден");
        }
        return parseEmployeeRow(result[0]);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении сотрудника: " << e.what() << "\n";
        throw;
    }
}

std::vector<Models::Employee> DatabaseManager::getAllEmployees() {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Employee> employees;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec(getQuery("employees_get_all"));
        for (auto row : result) {
            employees.push_back(parseEmployeeRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении сотрудников: " << e.what() << "\n";
        throw;
    }
    return employees;
}

bool DatabaseManager::deleteEmployee(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(getQuery("employees_delete"), id);
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при удалении сотрудника: " << e.what() << "\n";
        throw;
    }
}

// Competence operations
int DatabaseManager::addCompetence(const Models::Competence& competence) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            getQuery("competences_insert"),
            competence.getName(),
            competence.getDescription(),
            competence.getCategory()
        );
        txn.commit();
        return result[0]["id"].as<int>();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при добавлении компетенции: " << e.what() << "\n";
        throw;
    }
}

std::vector<Models::Competence> DatabaseManager::getAllCompetences() {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Competence> competences;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec(getQuery("competences_get_all"));
        for (auto row : result) {
            competences.push_back(parseCompetenceRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении компетенций: " << e.what() << "\n";
        throw;
    }
    return competences;
}

bool DatabaseManager::deleteCompetence(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(getQuery("competences_delete"), id);
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting competence: " << e.what() << "\n";
        throw;
    }
}

// Matrix operations
int DatabaseManager::addMatrix(const Models::Matrix& matrix) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            getQuery("matrices_insert"),
            matrix.getName(),
            matrix.getDescription()
        );
        txn.commit();
        return result[0]["id"].as<int>();
    } catch (const std::exception& e) {
        std::cerr << "Error adding matrix: " << e.what() << "\n";
        throw;
    }
}

Models::Matrix DatabaseManager::getMatrix(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(getQuery("matrices_get_by_id"), id);
        if (result.empty()) {
            throw std::runtime_error("Matrix not found");
        }
        return parseMatrixRow(result[0]);
    } catch (const std::exception& e) {
        std::cerr << "Error getting matrix: " << e.what() << "\n";
        throw;
    }
}

std::vector<Models::Matrix> DatabaseManager::getAllMatrices() {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Matrix> matrices;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec(getQuery("matrices_get_all"));
        for (auto row : result) {
            matrices.push_back(parseMatrixRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting matrices: " << e.what() << "\n";
        throw;
    }
    return matrices;
}

bool DatabaseManager::deleteMatrix(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(getQuery("matrices_delete"), id);
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting matrix: " << e.what() << "\n";
        throw;
    }
}

// Matrix-competence relations
bool DatabaseManager::addCompetencyToMatrix(int matrixId, int competenceId, int requiredLevel) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            getQuery("matrix_competencies_upsert"),
            matrixId, competenceId, requiredLevel
        );
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error adding competency to matrix: " << e.what() << "\n";
        throw;
    }
}

bool DatabaseManager::removeCompetencyFromMatrix(int matrixId, int competenceId) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            getQuery("matrix_competencies_delete"),
            matrixId, competenceId
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error removing competency from matrix: " << e.what() << "\n";
        throw;
    }
}

std::vector<std::pair<int, int>> DatabaseManager::getMatrixCompetencies(int matrixId) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<std::pair<int, int>> competencies;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            getQuery("matrix_competencies_get_by_matrix"),
            matrixId
        );
        for (auto row : result) {
            competencies.push_back({
                row["competence_id"].as<int>(),
                row["required_level"].as<int>()
            });
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting matrix competencies: " << e.what() << "\n";
        throw;
    }
    return competencies;
}

// Assessment operations
int DatabaseManager::addAssessment(const Models::Assessment& assessment) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            getQuery("assessments_insert"),
            assessment.getEmployeeId(),
            assessment.getCompetenceId(),
            assessment.getActualLevel(),
            assessment.getComments()
        );
        txn.commit();
        return result[0]["id"].as<int>();
    } catch (const std::exception& e) {
        std::cerr << "Error adding assessment: " << e.what() << "\n";
        throw;
    }
}

std::vector<Models::Assessment> DatabaseManager::getEmployeeAssessments(int employeeId) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Assessment> assessments;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            getQuery("assessments_get_by_employee"),
            employeeId
        );
        for (auto row : result) {
            assessments.push_back(parseAssessmentRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting employee assessments: " << e.what() << "\n";
        throw;
    }
    return assessments;
}

bool DatabaseManager::deleteAssessment(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(getQuery("assessments_delete"), id);
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting assessment: " << e.what() << "\n";
        throw;
    }
}

// Position operations
int DatabaseManager::addPosition(const Models::Position& position) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        if (position.getMatrixId() != -1) {
            auto result = txn.exec_params(
                getQuery("positions_insert_with_matrix"),
                position.getName(),
                position.getMatrixId()
            );
            txn.commit();
            return result[0]["id"].as<int>();
        } else {
            auto result = txn.exec_params(
                getQuery("positions_insert_without_matrix"),
                position.getName()
            );
            txn.commit();
            return result[0]["id"].as<int>();
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при добавлении должности: " << e.what() << "\n";
        throw;
    }
}

Models::Position DatabaseManager::getPosition(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(getQuery("positions_get_by_id"), id);
        if (result.empty()) {
            throw std::runtime_error("Должность не найдена");
        }
        return parsePositionRow(result[0]);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении должности: " << e.what() << "\n";
        throw;
    }
}

std::vector<Models::Position> DatabaseManager::getAllPositions() {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Position> positions;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec(getQuery("positions_get_all"));
        for (auto row : result) {
            positions.push_back(parsePositionRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении должностей: " << e.what() << "\n";
        throw;
    }
    return positions;
}

std::vector<Models::Position> DatabaseManager::getPositionsByMatrix(int matrixId) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Position> positions;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            getQuery("positions_get_by_matrix"),
            matrixId
        );
        for (auto row : result) {
            positions.push_back(parsePositionRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении должностей матрицы: " << e.what() << "\n";
        throw;
    }
    return positions;
}

bool DatabaseManager::updatePosition(const Models::Position& position) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        if (position.getMatrixId() != -1) {
            txn.exec_params(
                getQuery("positions_update_with_matrix"),
                position.getName(),
                position.getMatrixId(),
                position.getId()
            );
        } else {
            txn.exec_params(
                getQuery("positions_update_without_matrix"),
                position.getName(),
                position.getId()
            );
        }
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при обновлении должности: " << e.what() << "\n";
        throw;
    }
}

bool DatabaseManager::deletePosition(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        txn.exec_params(getQuery("positions_delete"), id);
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при удалении должности: " << e.what() << "\n";
        throw;
    }
}

// Helper methods
Models::Employee DatabaseManager::parseEmployeeRow(const pqxx::row& row) {
    int positionId = -1;
    if (!row["position_id"].is_null()) {
        positionId = row["position_id"].as<int>();
    }
    Models::Employee emp(
        row["id"].as<int>(),
        row["name"].as<std::string>(),
        positionId,
        row["department"].as<std::string>(),
        row["email"].as<std::string>()
    );
    return emp;
}

Models::Competence DatabaseManager::parseCompetenceRow(const pqxx::row& row) {
    Models::Competence comp(
        row["id"].as<int>(),
        row["name"].as<std::string>(),
        row["description"].as<std::string>(),
        row["category"].as<std::string>()
    );
    return comp;
}

Models::Position DatabaseManager::parsePositionRow(const pqxx::row& row) {
    int matrixId = -1;
    if (!row["matrix_id"].is_null()) {
        matrixId = row["matrix_id"].as<int>();
    }
    Models::Position pos(
        row["id"].as<int>(),
        row["name"].as<std::string>(),
        matrixId
    );
    return pos;
}

Models::Matrix DatabaseManager::parseMatrixRow(const pqxx::row& row) {
    Models::Matrix matrix(
        row["id"].as<int>(),
        row["name"].as<std::string>(),
        row["description"].as<std::string>()
    );
    return matrix;
}

Models::Assessment DatabaseManager::parseAssessmentRow(const pqxx::row& row) {
    Models::Assessment assess(
        row["id"].as<int>(),
        row["employee_id"].as<int>(),
        row["competence_id"].as<int>(),
        row["actual_level"].as<int>(),
        row["comments"].as<std::string>()
    );
    return assess;
}

} // namespace Database
