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

private:
    std::shared_ptr<Database::DatabaseManager> db;

    // Helper methods
    int getEmployeeLevel(int employeeId, int competenceId);
    bool isCompetenceMet(int actualLevel, int requiredLevel);
};

} // namespace Services
