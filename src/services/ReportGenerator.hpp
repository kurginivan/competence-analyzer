#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <memory>
#include "AnalysisEngine.hpp"

namespace Services {

class ReportGenerator {
public:
    ReportGenerator(std::shared_ptr<AnalysisEngine> engine);

    // Employee compliance report
    void printEmployeeComplianceReport(const ComplianceReport& report);
    void printMatrixComplianceReport(const std::vector<ComplianceReport>& reports);

    // Gap analysis report
    void printGapAnalysisReport(int employeeId, const std::vector<GapAnalysis>& gaps);
    void printMatrixGapAnalysisReport(int matrixId, const std::map<int, std::vector<GapAnalysis>>& allGaps);

    // Summary statistics
    void printMatrixStatistics(int matrixId);
    void printCompetenceStatistics(int matrixId);
    void printEmployeeStatistics(int employeeId);

    // Detailed reports
    void printDetailedComplianceAnalysis(int employeeId, int matrixId);
    void printMatrixOverallAnalysis(int matrixId);

    // Export functions (for future GUI/file export)
    std::string generateComplianceReportText(const ComplianceReport& report);
    std::string generateGapAnalysisText(int employeeId, const std::vector<GapAnalysis>& gaps);

private:
    std::shared_ptr<AnalysisEngine> engine;

    // Helper methods
    void printSeparator(int width = 80);
    void printHeader(const std::string& title, int width = 80);
    void printProgressBar(double percentage, int width = 40);
    std::string levelToString(int level);
    std::string complianceToString(double percentage);
};

} // namespace Services
