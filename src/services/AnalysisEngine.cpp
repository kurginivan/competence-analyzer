#include "AnalysisEngine.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace Services {

AnalysisEngine::AnalysisEngine(std::shared_ptr<Database::DatabaseManager> db)
    : db(db) {}

ComplianceReport AnalysisEngine::analyzeEmployeeCompliance(int employeeId, int matrixId) {
    ComplianceReport report;
    report.employeeId = employeeId;
    report.matrixId = matrixId;
    report.metCompetencies = 0;
    report.compliancePercentage = 0.0;

    try {
        // Get matrix competencies
        auto matrixComps = db->getMatrixCompetencies(matrixId);
        report.totalCompetencies = matrixComps.size();

        if (report.totalCompetencies == 0) {
            report.compliancePercentage = 100.0;
            return report;
        }

        // Analyze each competency
        for (const auto& [competenceId, requiredLevel] : matrixComps) {
            int actualLevel = getEmployeeLevel(employeeId, competenceId);

            if (isCompetenceMet(actualLevel, requiredLevel)) {
                report.metCompetencies++;
            } else {
                GapAnalysis gap;
                gap.employeeId = employeeId;
                gap.competenceId = competenceId;
                gap.requiredLevel = requiredLevel;
                gap.actualLevel = actualLevel;
                gap.gap = requiredLevel - actualLevel;
                report.gaps.push_back(gap);
            }
        }

        // Calculate compliance percentage
        report.compliancePercentage = (report.totalCompetencies > 0)
            ? (static_cast<double>(report.metCompetencies) / report.totalCompetencies) * 100.0
            : 0.0;

    } catch (const std::exception& e) {
        std::cerr << "Ошибка при анализе соответствия сотрудника: " << e.what() << "\n";
    }

    return report;
}

std::vector<ComplianceReport> AnalysisEngine::analyzeAllEmployeeCompliance(int matrixId) {
    std::vector<ComplianceReport> reports;

    try {
        auto employees = db->getAllEmployees();
        for (const auto& emp : employees) {
            reports.push_back(analyzeEmployeeCompliance(emp.getId(), matrixId));
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при анализе всех сотрудников: " << e.what() << "\n";
    }

    return reports;
}

std::vector<GapAnalysis> AnalysisEngine::getEmployeeGaps(int employeeId, int matrixId) {
    auto report = analyzeEmployeeCompliance(employeeId, matrixId);
    return report.gaps;
}

std::map<int, std::vector<GapAnalysis>> AnalysisEngine::getAllGaps(int matrixId) {
    std::map<int, std::vector<GapAnalysis>> allGaps;

    try {
        auto employees = db->getAllEmployees();
        for (const auto& emp : employees) {
            auto gaps = getEmployeeGaps(emp.getId(), matrixId);
            if (!gaps.empty()) {
                allGaps[emp.getId()] = gaps;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении всех пробелов: " << e.what() << "\n";
    }

    return allGaps;
}

double AnalysisEngine::getAverageComplianceForMatrix(int matrixId) {
    auto reports = analyzeAllEmployeeCompliance(matrixId);
    
    if (reports.empty()) return 0.0;

    double totalCompliance = 0.0;
    for (const auto& report : reports) {
        totalCompliance += report.compliancePercentage;
    }

    return totalCompliance / reports.size();
}

int AnalysisEngine::getCompetentEmployeeCount(int matrixId) {
    auto reports = analyzeAllEmployeeCompliance(matrixId);
    int count = 0;

    for (const auto& report : reports) {
        if (report.compliancePercentage >= 100.0) {
            count++;
        }
    }

    return count;
}

std::map<int, double> AnalysisEngine::getCompetenceAverageLevel(int matrixId) {
    std::map<int, double> averageLevels;
    std::map<int, std::vector<int>> levelsByCompetence;

    try {
        auto employees = db->getAllEmployees();
        auto matrixComps = db->getMatrixCompetencies(matrixId);

        for (const auto& [competenceId, _] : matrixComps) {
            for (const auto& emp : employees) {
                int level = getEmployeeLevel(emp.getId(), competenceId);
                if (level > 0) {
                    levelsByCompetence[competenceId].push_back(level);
                }
            }

            if (!levelsByCompetence[competenceId].empty()) {
                double sum = 0;
                for (int level : levelsByCompetence[competenceId]) {
                    sum += level;
                }
                averageLevels[competenceId] = sum / levelsByCompetence[competenceId].size();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при расчете средних уровней компетенций: " << e.what() << "\n";
    }

    return averageLevels;
}

double AnalysisEngine::getEmployeeAverageLevel(int employeeId) {
    try {
        auto assessments = db->getEmployeeAssessments(employeeId);
        if (assessments.empty()) return 0.0;

        double sum = 0;
        for (const auto& assessment : assessments) {
            sum += assessment.getActualLevel();
        }

        return sum / assessments.size();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при расчете среднего уровня сотрудника: " << e.what() << "\n";
        return 0.0;
    }
}

std::map<int, double> AnalysisEngine::getEmployeeCompetenceMap(int employeeId) {
    std::map<int, double> competenceMap;

    try {
        auto assessments = db->getEmployeeAssessments(employeeId);
        for (const auto& assessment : assessments) {
            competenceMap[assessment.getCompetenceId()] = assessment.getActualLevel();
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении карты компетенций сотрудника: " << e.what() << "\n";
    }

    return competenceMap;
}

int AnalysisEngine::getEmployeeLevel(int employeeId, int competenceId) {
    try {
        auto assessments = db->getEmployeeAssessments(employeeId);
        
        // Find the most recent assessment for this competence
        for (const auto& assessment : assessments) {
            if (assessment.getCompetenceId() == competenceId) {
                return assessment.getActualLevel();
            }
        }
        
        return 0; // No assessment found
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении уровня сотрудника: " << e.what() << "\n";
        return 0;
    }
}

bool AnalysisEngine::isCompetenceMet(int actualLevel, int requiredLevel) {
    return actualLevel >= requiredLevel;
}

} // namespace Services
