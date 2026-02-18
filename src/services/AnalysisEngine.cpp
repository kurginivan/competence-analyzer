#include "AnalysisEngine.hpp"
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
