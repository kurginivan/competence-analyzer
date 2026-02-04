#pragma once

#include <vector>
#include <map>
#include <memory>
#include "../database/DatabaseManager.hpp"

namespace Services {

// Structure to hold gap analysis results
struct GapAnalysis {
    int employeeId;
    int competenceId;
    int requiredLevel;
    int actualLevel;
    int gap; // requiredLevel - actualLevel
};

// Structure to hold compliance analysis
struct ComplianceReport {
    int employeeId;
    int matrixId;
    int totalCompetencies;
    int metCompetencies;
    double compliancePercentage;
    std::vector<GapAnalysis> gaps;
};

class AnalysisEngine {
public:
    AnalysisEngine(std::shared_ptr<Database::DatabaseManager> db);

    // Compliance analysis
    ComplianceReport analyzeEmployeeCompliance(int employeeId, int matrixId);
    std::vector<ComplianceReport> analyzeAllEmployeeCompliance(int matrixId);

    // Gap analysis
    std::vector<GapAnalysis> getEmployeeGaps(int employeeId, int matrixId);
    std::map<int, std::vector<GapAnalysis>> getAllGaps(int matrixId);

    // Statistics
    double getAverageComplianceForMatrix(int matrixId);
    int getCompetentEmployeeCount(int matrixId); // Count of 100% compliant employees
    std::map<int, double> getCompetenceAverageLevel(int matrixId);

    // Competence level analysis
    double getEmployeeAverageLevel(int employeeId);
    std::map<int, double> getEmployeeCompetenceMap(int employeeId);

private:
    std::shared_ptr<Database::DatabaseManager> db;

    // Helper methods
    int getEmployeeLevel(int employeeId, int competenceId);
    bool isCompetenceMet(int actualLevel, int requiredLevel);
};

} // namespace Services
