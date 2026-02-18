#include <crow.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "database/DatabaseManager.hpp"

using json = nlohmann::json;

namespace {

std::string getEnvOrDefault(const char* name, const std::string& fallback) {
    const char* value = std::getenv(name);
    return value ? std::string(value) : fallback;
}

std::string buildConnectionString() {
    const std::string host = getEnvOrDefault("DATABASE_HOST", "localhost");
    const std::string port = getEnvOrDefault("DATABASE_PORT", "5432");
    const std::string user = getEnvOrDefault("DATABASE_USER", "postgres");
    const std::string password = getEnvOrDefault("DATABASE_PASSWORD", "postgres");
    const std::string dbname = getEnvOrDefault("DATABASE_NAME", "competence_analyzer");

    std::ostringstream oss;
    oss << "user=" << user
        << " password=" << password
        << " host=" << host
        << " port=" << port
        << " dbname=" << dbname
        << " connect_timeout=10";
    return oss.str();
}

std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

crow::response serveStatic(const std::string& staticDir, const std::string& fileName, const std::string& contentType) {
    std::string content = readFile(staticDir + "/" + fileName);
    if (content.empty()) {
        return crow::response(404);
    }
    crow::response res{content};
    res.set_header("Content-Type", contentType);
    return res;
}

crow::response jsonResponse(const json& payload, int status = 200) {
    crow::response res{status};
    res.set_header("Content-Type", "application/json");
    res.write(payload.dump());
    return res;
}

json employeeToJson(const Models::Employee& emp, const std::unordered_map<int, Models::Position>& positions) {
    json result;
    result["id"] = emp.getId();
    result["name"] = emp.getName();
    result["department"] = emp.getDepartment();
    result["email"] = emp.getEmail();

    if (emp.getPositionId() == -1) {
        result["position_id"] = nullptr;
        result["position_name"] = nullptr;
        result["position_matrix_id"] = nullptr;
        return result;
    }

    result["position_id"] = emp.getPositionId();
    auto it = positions.find(emp.getPositionId());
    if (it != positions.end()) {
        result["position_name"] = it->second.getName();
        if (it->second.getMatrixId() == -1) {
            result["position_matrix_id"] = nullptr;
        } else {
            result["position_matrix_id"] = it->second.getMatrixId();
        }
    } else {
        result["position_name"] = nullptr;
        result["position_matrix_id"] = nullptr;
    }

    return result;
}

json positionToJson(const Models::Position& pos, const std::unordered_map<int, Models::Matrix>& matrices) {
    json result;
    result["id"] = pos.getId();
    result["name"] = pos.getName();
    if (pos.getMatrixId() == -1) {
        result["matrix_id"] = nullptr;
        result["matrix_name"] = nullptr;
    } else {
        result["matrix_id"] = pos.getMatrixId();
        auto it = matrices.find(pos.getMatrixId());
        result["matrix_name"] = (it != matrices.end()) ? it->second.getName() : nullptr;
    }
    return result;
}

json competenceToJson(const Models::Competence& comp) {
    json result;
    result["id"] = comp.getId();
    result["name"] = comp.getName();
    result["description"] = comp.getDescription();
    result["category"] = comp.getCategory();
    return result;
}

json matrixToJson(const Models::Matrix& matrix) {
    json result;
    result["id"] = matrix.getId();
    result["name"] = matrix.getName();
    result["description"] = matrix.getDescription();
    return result;
}

std::unordered_map<int, Models::Position> loadPositions(Database::DatabaseManager& db) {
    std::unordered_map<int, Models::Position> map;
    for (const auto& pos : db.getAllPositions()) {
        map.emplace(pos.getId(), pos);
    }
    return map;
}

std::unordered_map<int, Models::Matrix> loadMatrices(Database::DatabaseManager& db) {
    std::unordered_map<int, Models::Matrix> map;
    for (const auto& matrix : db.getAllMatrices()) {
        map.emplace(matrix.getId(), matrix);
    }
    return map;
}

std::unordered_map<int, Models::Competence> loadCompetences(Database::DatabaseManager& db) {
    std::unordered_map<int, Models::Competence> map;
    for (const auto& comp : db.getAllCompetences()) {
        map.emplace(comp.getId(), comp);
    }
    return map;
}

json buildAverages(Database::DatabaseManager& db, int employeeId, const std::unordered_map<int, Models::Competence>& competences) {
    std::unordered_map<int, std::pair<int, int>> sums;
    for (const auto& assess : db.getEmployeeAssessments(employeeId)) {
        auto& entry = sums[assess.getCompetenceId()];
        entry.first += assess.getActualLevel();
        entry.second += 1;
    }

    json averages = json::array();
    for (const auto& [competenceId, pair] : sums) {
        double avg = static_cast<double>(pair.first) / static_cast<double>(pair.second);
        json item;
        item["competence_id"] = competenceId;
        auto it = competences.find(competenceId);
        item["competence_name"] = (it != competences.end()) ? it->second.getName() : "";
        item["avg_level"] = std::round(avg * 100.0) / 100.0;
        averages.push_back(item);
    }

    std::sort(averages.begin(), averages.end(), [](const json& a, const json& b) {
        return a.at("avg_level").get<double>() < b.at("avg_level").get<double>();
    });

    return averages;
}

json buildGaps(const json& averages) {
    json gaps = json::array();
    for (const auto& item : averages) {
        if (item.at("avg_level").get<double>() < 3.0) {
            gaps.push_back(item);
        }
    }
    return gaps;
}

json buildRecommendations(const json& averages) {
    json recs = json::array();
    for (size_t i = 0; i < averages.size() && i < 5; ++i) {
        recs.push_back(averages[i]);
    }
    return recs;
}

json buildUnmetCompetencies(const std::vector<std::pair<int, int>>& requirements,
                            const json& averages,
                            const std::unordered_map<int, Models::Competence>& competences) {
    std::unordered_map<int, double> avgMap;
    for (const auto& item : averages) {
        avgMap[item.at("competence_id").get<int>()] = item.at("avg_level").get<double>();
    }

    json unmet = json::array();
    for (const auto& req : requirements) {
        const int competenceId = req.first;
        const int requiredLevel = req.second;
        const double actualLevel = avgMap.count(competenceId) ? avgMap[competenceId] : 0.0;
        if (actualLevel < requiredLevel) {
            json row;
            row["competence_id"] = competenceId;
            auto it = competences.find(competenceId);
            row["competence_name"] = (it != competences.end()) ? it->second.getName() : "";
            row["required_level"] = requiredLevel;
            row["actual_level"] = actualLevel;
            unmet.push_back(row);
        }
    }
    return unmet;
}

double computeMatchPercentage(const std::vector<std::pair<int, int>>& requirements,
                             const json& averages) {
    if (requirements.empty()) {
        return -1.0;
    }
    std::unordered_map<int, double> avgMap;
    for (const auto& item : averages) {
        avgMap[item.at("competence_id").get<int>()] = item.at("avg_level").get<double>();
    }

    int met = 0;
    for (const auto& req : requirements) {
        const int competenceId = req.first;
        const int requiredLevel = req.second;
        const double actualLevel = avgMap.count(competenceId) ? avgMap[competenceId] : 0.0;
        if (actualLevel >= requiredLevel) {
            met += 1;
        }
    }

    return std::round((static_cast<double>(met) / requirements.size()) * 100.0 * 100.0) / 100.0;
}

} // namespace

int main() {
    const std::string connStr = buildConnectionString();
    auto db = std::make_shared<Database::DatabaseManager>(connStr);
    if (!db->connect()) {
        std::cerr << "Database connection failed." << std::endl;
        return 1;
    }

    const std::string staticDir = getEnvOrDefault("COMP_ANALYZER_WEB_STATIC", "web/static");

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([&]() {
        return serveStatic(staticDir, "index.html", "text/html; charset=utf-8");
    });

    CROW_ROUTE(app, "/style.css")([&]() {
        return serveStatic(staticDir, "style.css", "text/css; charset=utf-8");
    });

    CROW_ROUTE(app, "/script.js")([&]() {
        return serveStatic(staticDir, "script.js", "application/javascript; charset=utf-8");
    });

    CROW_ROUTE(app, "/api/health")([&]() {
        json payload;
        payload["status"] = "ok";
        payload["database"] = "connected";
        return jsonResponse(payload);
    });

    CROW_ROUTE(app, "/api/employees").methods(crow::HTTPMethod::GET)([&]() {
        try {
            auto positions = loadPositions(*db);
            json employees = json::array();
            for (const auto& emp : db->getAllEmployees()) {
                employees.push_back(employeeToJson(emp, positions));
            }
            return jsonResponse(employees);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/employees/<int>").methods(crow::HTTPMethod::GET)([&](int empId) {
        try {
            auto positions = loadPositions(*db);
            auto emp = db->getEmployee(empId);
            return jsonResponse(employeeToJson(emp, positions));
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/employees").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
        try {
            const auto body = json::parse(req.body);
            const std::string name = body.value("name", "");
            const std::string department = body.value("department", "");
            const std::string email = body.value("email", "");
            const int positionId = body.contains("position_id") && !body["position_id"].is_null()
                ? body["position_id"].get<int>()
                : -1;

            Models::Employee employee(0, name, positionId, department, email);
            const int id = db->addEmployee(employee);
            auto positions = loadPositions(*db);
            auto created = db->getEmployee(id);
            return jsonResponse(employeeToJson(created, positions));
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/employees/<int>").methods(crow::HTTPMethod::DELETE)([&](int empId) {
        try {
            db->deleteEmployee(empId);
            return jsonResponse({{"status", "deleted"}});
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/positions").methods(crow::HTTPMethod::GET)([&]() {
        try {
            auto matrices = loadMatrices(*db);
            json positions = json::array();
            for (const auto& pos : db->getAllPositions()) {
                positions.push_back(positionToJson(pos, matrices));
            }
            return jsonResponse(positions);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/positions").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
        try {
            const auto body = json::parse(req.body);
            const std::string name = body.value("name", "");
            const int matrixId = body.contains("matrix_id") && !body["matrix_id"].is_null()
                ? body["matrix_id"].get<int>()
                : -1;
            Models::Position position(0, name, matrixId);
            const int id = db->addPosition(position);
            auto matrices = loadMatrices(*db);
            auto created = db->getPosition(id);
            return jsonResponse(positionToJson(created, matrices));
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/positions/<int>").methods(crow::HTTPMethod::PUT)([&](const crow::request& req, int posId) {
        try {
            const auto body = json::parse(req.body);
            const std::string name = body.value("name", "");
            const int matrixId = body.contains("matrix_id") && !body["matrix_id"].is_null()
                ? body["matrix_id"].get<int>()
                : -1;
            Models::Position position(posId, name, matrixId);
            db->updatePosition(position);
            auto matrices = loadMatrices(*db);
            auto updated = db->getPosition(posId);
            return jsonResponse(positionToJson(updated, matrices));
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/positions/by-matrix/<int>").methods(crow::HTTPMethod::GET)([&](int matrixId) {
        try {
            auto matrices = loadMatrices(*db);
            json positions = json::array();
            for (const auto& pos : db->getPositionsByMatrix(matrixId)) {
                positions.push_back(positionToJson(pos, matrices));
            }
            return jsonResponse(positions);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/positions/<int>").methods(crow::HTTPMethod::DELETE)([&](int posId) {
        try {
            db->deletePosition(posId);
            return jsonResponse({{"status", "deleted"}});
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/competences").methods(crow::HTTPMethod::GET)([&]() {
        try {
            json competences = json::array();
            for (const auto& comp : db->getAllCompetences()) {
                competences.push_back(competenceToJson(comp));
            }
            return jsonResponse(competences);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/competences").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
        try {
            const auto body = json::parse(req.body);
            Models::Competence comp(
                0,
                body.value("name", ""),
                body.value("description", ""),
                body.value("category", "General")
            );
            const int id = db->addCompetence(comp);
            json payload = competenceToJson(comp);
            payload["id"] = id;
            return jsonResponse(payload);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/competences/<int>").methods(crow::HTTPMethod::DELETE)([&](int compId) {
        try {
            db->deleteCompetence(compId);
            return jsonResponse({{"status", "deleted"}});
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/matrices").methods(crow::HTTPMethod::GET)([&]() {
        try {
            json matrices = json::array();
            for (const auto& matrix : db->getAllMatrices()) {
                matrices.push_back(matrixToJson(matrix));
            }
            return jsonResponse(matrices);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/matrices").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
        try {
            const auto body = json::parse(req.body);
            Models::Matrix matrix(0, body.value("name", ""), body.value("description", ""));
            const int id = db->addMatrix(matrix);
            json payload = matrixToJson(matrix);
            payload["id"] = id;
            return jsonResponse(payload);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/matrices/<int>").methods(crow::HTTPMethod::DELETE)([&](int matrixId) {
        try {
            db->deleteMatrix(matrixId);
            return jsonResponse({{"status", "deleted"}});
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/matrices/<int>/competencies").methods(crow::HTTPMethod::GET)([&](int matrixId) {
        try {
            auto competences = loadCompetences(*db);
            json items = json::array();
            for (const auto& pair : db->getMatrixCompetencies(matrixId)) {
                json item;
                item["competence_id"] = pair.first;
                item["required_level"] = pair.second;
                auto it = competences.find(pair.first);
                item["competence_name"] = (it != competences.end()) ? it->second.getName() : "";
                items.push_back(item);
            }
            return jsonResponse(items);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/matrices/<int>/competencies").methods(crow::HTTPMethod::POST)([&](const crow::request& req, int matrixId) {
        try {
            const auto body = json::parse(req.body);
            const int competenceId = body.value("competence_id", 0);
            const int requiredLevel = body.value("required_level", 1);
            db->addCompetencyToMatrix(matrixId, competenceId, requiredLevel);
            json payload;
            payload["id"] = 0;
            payload["competence_id"] = competenceId;
            payload["required_level"] = requiredLevel;
            return jsonResponse(payload);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/matrices/<int>/competencies/<int>").methods(crow::HTTPMethod::DELETE)([&](int matrixId, int competenceId) {
        try {
            db->removeCompetencyFromMatrix(matrixId, competenceId);
            return jsonResponse({{"status", "deleted"}});
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/assessments").methods(crow::HTTPMethod::GET)([&](const crow::request& req) {
        try {
            const char* positionParam = req.url_params.get("position_id");
            int filterPosition = positionParam ? std::atoi(positionParam) : 0;

            auto positions = loadPositions(*db);
            auto competences = loadCompetences(*db);

            json assessments = json::array();
            for (const auto& emp : db->getAllEmployees()) {
                if (filterPosition > 0 && emp.getPositionId() != filterPosition) {
                    continue;
                }
                for (const auto& assess : db->getEmployeeAssessments(emp.getId())) {
                    json item;
                    item["id"] = assess.getId();
                    item["employee_id"] = assess.getEmployeeId();
                    item["competence_id"] = assess.getCompetenceId();
                    item["actual_level"] = assess.getActualLevel();
                    item["comments"] = assess.getComments();
                    item["employee_name"] = emp.getName();
                    if (emp.getPositionId() == -1) {
                        item["position_id"] = nullptr;
                        item["position_name"] = nullptr;
                    } else {
                        item["position_id"] = emp.getPositionId();
                        auto posIt = positions.find(emp.getPositionId());
                        item["position_name"] = (posIt != positions.end()) ? posIt->second.getName() : "";
                    }
                    auto compIt = competences.find(assess.getCompetenceId());
                    item["competence_name"] = (compIt != competences.end()) ? compIt->second.getName() : "";
                    assessments.push_back(item);
                }
            }
            return jsonResponse(assessments);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/assessments").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
        try {
            const auto body = json::parse(req.body);
            Models::Assessment assessment(
                0,
                body.value("employee_id", 0),
                body.value("competence_id", 0),
                body.value("actual_level", 1),
                body.value("comments", "")
            );
            const int id = db->addAssessment(assessment);
            auto emp = db->getEmployee(assessment.getEmployeeId());
            auto competences = loadCompetences(*db);
            json payload;
            payload["id"] = id;
            payload["employee_id"] = assessment.getEmployeeId();
            payload["competence_id"] = assessment.getCompetenceId();
            payload["actual_level"] = assessment.getActualLevel();
            payload["comments"] = assessment.getComments();
            payload["employee_name"] = emp.getName();
            auto compIt = competences.find(assessment.getCompetenceId());
            payload["competence_name"] = (compIt != competences.end()) ? compIt->second.getName() : "";
            return jsonResponse(payload);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/assessments/<int>").methods(crow::HTTPMethod::DELETE)([&](int assId) {
        try {
            db->deleteAssessment(assId);
            return jsonResponse({{"status", "deleted"}});
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/analysis/employee/<int>").methods(crow::HTTPMethod::GET)([&](int empId) {
        try {
            auto positions = loadPositions(*db);
            auto competences = loadCompetences(*db);
            auto emp = db->getEmployee(empId);
            json empJson = employeeToJson(emp, positions);

            json averages = buildAverages(*db, empId, competences);
            json gaps = buildGaps(averages);
            json recommendations = buildRecommendations(averages);

            json response;
            response["employee"] = empJson;
            response["averages"] = averages;
            response["gaps"] = gaps;
            response["recommendations"] = recommendations;
            response["position_match_percentage"] = nullptr;

            if (emp.getPositionId() != -1) {
                auto posIt = positions.find(emp.getPositionId());
                if (posIt != positions.end() && posIt->second.getMatrixId() != -1) {
                    auto requirements = db->getMatrixCompetencies(posIt->second.getMatrixId());
                    const double match = computeMatchPercentage(requirements, averages);
                    if (match >= 0.0) {
                        response["position_match_percentage"] = match;
                    }
                }
            }

            return jsonResponse(response);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/employees/<int>/available-matrices").methods(crow::HTTPMethod::GET)([&](int empId) {
        try {
            auto emp = db->getEmployee(empId);
            auto matrices = loadMatrices(*db);

            json items = json::array();
            if (emp.getPositionId() != -1) {
                auto position = db->getPosition(emp.getPositionId());
                if (position.getMatrixId() != -1) {
                    auto it = matrices.find(position.getMatrixId());
                    if (it != matrices.end()) {
                        items.push_back(matrixToJson(it->second));
                    }
                }
            }

            if (items.empty()) {
                for (const auto& kv : matrices) {
                    items.push_back(matrixToJson(kv.second));
                }
            }

            json response;
            if (emp.getPositionId() == -1) {
                response["position_id"] = nullptr;
            } else {
                response["position_id"] = emp.getPositionId();
            }
            response["matrices"] = items;
            return jsonResponse(response);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/employees/<int>/analyze").methods(crow::HTTPMethod::POST)([&](const crow::request& req, int empId) {
        try {
            const auto body = json::parse(req.body);
            const int matrixId = body.value("matrix_id", 0);
            if (matrixId <= 0) {
                return jsonResponse({{"detail", "matrix_id required"}}, 400);
            }

            auto positions = loadPositions(*db);
            auto competences = loadCompetences(*db);
            auto emp = db->getEmployee(empId);
            json empJson = employeeToJson(emp, positions);

            json averages = buildAverages(*db, empId, competences);
            json gaps = buildGaps(averages);
            json recommendations = buildRecommendations(averages);

            auto requirements = db->getMatrixCompetencies(matrixId);
            double match = computeMatchPercentage(requirements, averages);
            json unmet = buildUnmetCompetencies(requirements, averages, competences);

            json response;
            response["employee"] = empJson;
            response["matrix_id"] = matrixId;
            response["averages"] = averages;
            response["gaps"] = gaps;
            response["recommendations"] = recommendations;
            if (match >= 0.0) {
                response["match_percentage"] = match;
            } else {
                response["match_percentage"] = nullptr;
            }
            response["unmet_competencies"] = unmet;
            return jsonResponse(response);
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    CROW_ROUTE(app, "/api/reports/employee/<int>").methods(crow::HTTPMethod::POST)([&](const crow::request& req, int empId) {
        try {
            const char* matrixParam = req.url_params.get("matrix_id");
            int matrixId = matrixParam ? std::atoi(matrixParam) : 0;

            auto positions = loadPositions(*db);
            auto competences = loadCompetences(*db);
            auto emp = db->getEmployee(empId);
            json averages = buildAverages(*db, empId, competences);
            json gaps = buildGaps(averages);
            json recommendations = buildRecommendations(averages);

            double match = -1.0;
            if (matrixId > 0) {
                auto requirements = db->getMatrixCompetencies(matrixId);
                match = computeMatchPercentage(requirements, averages);
            } else if (emp.getPositionId() != -1) {
                auto posIt = positions.find(emp.getPositionId());
                if (posIt != positions.end() && posIt->second.getMatrixId() != -1) {
                    auto requirements = db->getMatrixCompetencies(posIt->second.getMatrixId());
                    match = computeMatchPercentage(requirements, averages);
                }
            }

            std::ostringstream report;
            std::string positionName = "No position";
            if (emp.getPositionId() != -1) {
                auto posIt = positions.find(emp.getPositionId());
                if (posIt != positions.end()) {
                    positionName = posIt->second.getName();
                }
            }

            report << "Employee report: " << emp.getName() << " (" << positionName << " - " << emp.getDepartment() << ")\n";
            if (match >= 0.0) {
                report << "Matrix match: " << match << "%\n";
            }
            report << "\nAverage levels by competence:\n";
            for (const auto& item : averages) {
                report << "- " << item.at("competence_name").get<std::string>() << ": "
                       << item.at("avg_level").get<double>() << "/5\n";
            }
            report << "\nGaps (level < 3):\n";
            if (gaps.empty()) {
                report << "- No gaps detected\n";
            } else {
                for (const auto& item : gaps) {
                    report << "- " << item.at("competence_name").get<std::string>() << ": "
                           << item.at("avg_level").get<double>() << "/5\n";
                }
            }
            report << "\nRecommendations (top 5):\n";
            for (const auto& item : recommendations) {
                report << "- " << item.at("competence_name").get<std::string>() << ": "
                       << item.at("avg_level").get<double>() << "/5\n";
            }

            return jsonResponse({{"report", report.str()}});
        } catch (const std::exception& e) {
            return jsonResponse({{"detail", e.what()}}, 500);
        }
    });

    app.port(8000).run();
    return 0;
}
