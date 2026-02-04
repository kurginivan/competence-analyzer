#include "DatabaseManager.hpp"
#include <iostream>
#include <stdexcept>

namespace Database {

DatabaseManager::DatabaseManager(const std::string& connectionString)
    : connectionString(connectionString), connected(false) {}

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

// ==================== Employee Operations ====================

int DatabaseManager::addEmployee(const Models::Employee& employee) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "INSERT INTO employees (name, position, department, email) "
            "VALUES ($1, $2, $3, $4) RETURNING id",
            employee.getName(),
            employee.getPosition(),
            employee.getDepartment(),
            employee.getEmail()
        );
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
        auto result = txn.exec_params(
            "SELECT id, name, position, department, email, created_at, updated_at "
            "FROM employees WHERE id = $1",
            id
        );
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
        auto result = txn.exec(
            "SELECT id, name, position, department, email, created_at, updated_at "
            "FROM employees ORDER BY id"
        );
        for (auto row : result) {
            employees.push_back(parseEmployeeRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении сотрудников: " << e.what() << "\n";
        throw;
    }
    return employees;
}

bool DatabaseManager::updateEmployee(const Models::Employee& employee) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "UPDATE employees SET name = $1, position = $2, department = $3, "
            "email = $4, updated_at = CURRENT_TIMESTAMP WHERE id = $5",
            employee.getName(),
            employee.getPosition(),
            employee.getDepartment(),
            employee.getEmail(),
            employee.getId()
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при обновлении сотрудника: " << e.what() << "\n";
        throw;
    }
}

bool DatabaseManager::deleteEmployee(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "DELETE FROM employees WHERE id = $1",
            id
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при удалении сотрудника: " << e.what() << "\n";
        throw;
    }
}

// ==================== Competence Operations ====================

int DatabaseManager::addCompetence(const Models::Competence& competence) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "INSERT INTO competences (name, description, category) "
            "VALUES ($1, $2, $3) RETURNING id",
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

Models::Competence DatabaseManager::getCompetence(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "SELECT id, name, description, category, created_at, updated_at "
            "FROM competences WHERE id = $1",
            id
        );
        if (result.empty()) {
            throw std::runtime_error("Компетенция не найдена");
        }
        return parseCompetenceRow(result[0]);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении компетенции: " << e.what() << "\n";
        throw;
    }
}

std::vector<Models::Competence> DatabaseManager::getAllCompetences() {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Competence> competences;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec(
            "SELECT id, name, description, category, created_at, updated_at "
            "FROM competences ORDER BY category, name"
        );
        for (auto row : result) {
            competences.push_back(parseCompetenceRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении компетенций: " << e.what() << "\n";
        throw;
    }
    return competences;
}

std::vector<Models::Competence> DatabaseManager::getCompetencesByCategory(const std::string& category) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Competence> competences;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "SELECT id, name, description, category, created_at, updated_at "
            "FROM competences WHERE category = $1 ORDER BY name",
            category
        );
        for (auto row : result) {
            competences.push_back(parseCompetenceRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting competences by category: " << e.what() << "\n";
        throw;
    }
    return competences;
}

bool DatabaseManager::updateCompetence(const Models::Competence& competence) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "UPDATE competences SET name = $1, description = $2, category = $3, "
            "updated_at = CURRENT_TIMESTAMP WHERE id = $4",
            competence.getName(),
            competence.getDescription(),
            competence.getCategory(),
            competence.getId()
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error updating competence: " << e.what() << "\n";
        throw;
    }
}

bool DatabaseManager::deleteCompetence(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "DELETE FROM competences WHERE id = $1",
            id
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting competence: " << e.what() << "\n";
        throw;
    }
}

// ==================== Matrix Operations ====================

int DatabaseManager::addMatrix(const Models::Matrix& matrix) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "INSERT INTO matrices (name, description) "
            "VALUES ($1, $2) RETURNING id",
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
        auto result = txn.exec_params(
            "SELECT id, name, description, created_at, updated_at "
            "FROM matrices WHERE id = $1",
            id
        );
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
        auto result = txn.exec(
            "SELECT id, name, description, created_at, updated_at "
            "FROM matrices ORDER BY id"
        );
        for (auto row : result) {
            matrices.push_back(parseMatrixRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting matrices: " << e.what() << "\n";
        throw;
    }
    return matrices;
}

bool DatabaseManager::updateMatrix(const Models::Matrix& matrix) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "UPDATE matrices SET name = $1, description = $2, "
            "updated_at = CURRENT_TIMESTAMP WHERE id = $3",
            matrix.getName(),
            matrix.getDescription(),
            matrix.getId()
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error updating matrix: " << e.what() << "\n";
        throw;
    }
}

bool DatabaseManager::deleteMatrix(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "DELETE FROM matrices WHERE id = $1",
            id
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting matrix: " << e.what() << "\n";
        throw;
    }
}

// ==================== Matrix-Competence Relations ====================

bool DatabaseManager::addCompetencyToMatrix(int matrixId, int competenceId, int requiredLevel) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "INSERT INTO matrix_competencies (matrix_id, competence_id, required_level) "
            "VALUES ($1, $2, $3) ON CONFLICT (matrix_id, competence_id) "
            "DO UPDATE SET required_level = $3",
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
            "DELETE FROM matrix_competencies WHERE matrix_id = $1 AND competence_id = $2",
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
            "SELECT competence_id, required_level FROM matrix_competencies "
            "WHERE matrix_id = $1",
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

// ==================== Assessment Operations ====================

int DatabaseManager::addAssessment(const Models::Assessment& assessment) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "INSERT INTO assessments (employee_id, competence_id, actual_level, comments) "
            "VALUES ($1, $2, $3, $4) RETURNING id",
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

Models::Assessment DatabaseManager::getAssessment(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "SELECT id, employee_id, competence_id, actual_level, comments, "
            "assessment_date, created_at, updated_at FROM assessments WHERE id = $1",
            id
        );
        if (result.empty()) {
            throw std::runtime_error("Assessment not found");
        }
        return parseAssessmentRow(result[0]);
    } catch (const std::exception& e) {
        std::cerr << "Error getting assessment: " << e.what() << "\n";
        throw;
    }
}

std::vector<Models::Assessment> DatabaseManager::getEmployeeAssessments(int employeeId) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Assessment> assessments;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "SELECT id, employee_id, competence_id, actual_level, comments, "
            "assessment_date, created_at, updated_at FROM assessments "
            "WHERE employee_id = $1 ORDER BY assessment_date DESC",
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

std::vector<Models::Assessment> DatabaseManager::getCompetenceAssessments(int competenceId) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Assessment> assessments;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "SELECT id, employee_id, competence_id, actual_level, comments, "
            "assessment_date, created_at, updated_at FROM assessments "
            "WHERE competence_id = $1 ORDER BY assessment_date DESC",
            competenceId
        );
        for (auto row : result) {
            assessments.push_back(parseAssessmentRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting competence assessments: " << e.what() << "\n";
        throw;
    }
    return assessments;
}

bool DatabaseManager::updateAssessment(const Models::Assessment& assessment) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "UPDATE assessments SET actual_level = $1, comments = $2, "
            "updated_at = CURRENT_TIMESTAMP WHERE id = $3",
            assessment.getActualLevel(),
            assessment.getComments(),
            assessment.getId()
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error updating assessment: " << e.what() << "\n";
        throw;
    }
}

bool DatabaseManager::deleteAssessment(int id) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "DELETE FROM assessments WHERE id = $1",
            id
        );
        txn.commit();
        return result.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting assessment: " << e.what() << "\n";
        throw;
    }
}

std::vector<Models::Assessment> DatabaseManager::getLatestAssessments(int employeeId, int limit) {
    if (!isConnected()) throw std::runtime_error("Database not connected");

    std::vector<Models::Assessment> assessments;
    try {
        pqxx::work txn(*connection);
        auto result = txn.exec_params(
            "SELECT id, employee_id, competence_id, actual_level, comments, "
            "assessment_date, created_at, updated_at FROM assessments "
            "WHERE employee_id = $1 ORDER BY assessment_date DESC LIMIT $2",
            employeeId, limit
        );
        for (auto row : result) {
            assessments.push_back(parseAssessmentRow(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting latest assessments: " << e.what() << "\n";
        throw;
    }
    return assessments;
}

// ==================== Helper Methods ====================

Models::Employee DatabaseManager::parseEmployeeRow(const pqxx::row& row) {
    Models::Employee emp(
        row["id"].as<int>(),
        row["name"].as<std::string>(),
        row["position"].as<std::string>(),
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
