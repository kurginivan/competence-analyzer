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

private:
    std::shared_ptr<AnalysisEngine> engine;

    // Helper methods
    void printProgressBar(double percentage, int width = 40);
    std::string complianceToString(double percentage);
};

} // namespace Services
