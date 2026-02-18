#include "ReportGenerator.hpp"
#include <iostream>
#include <iomanip>

namespace Services {

ReportGenerator::ReportGenerator(std::shared_ptr<AnalysisEngine> engine)
    : engine(engine) {}

void ReportGenerator::printProgressBar(double percentage, int width) {
    int filled = static_cast<int>((percentage / 100.0) * width);
    std::cout << "[";
    for (int i = 0; i < width; i++) {
        if (i < filled) std::cout << "█";
        else std::cout << "░";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "%\n";
}

std::string ReportGenerator::complianceToString(double percentage) {
    if (percentage >= 100.0) return "Полностью соответствует";
    if (percentage >= 80.0) return "В основном соответствует";
    if (percentage >= 50.0) return "Частично соответствует";
    return "Не соответствует";
}

void ReportGenerator::printEmployeeComplianceReport(const ComplianceReport& report) {
    std::cout << "\nID сотрудника: " << report.employeeId << "\n";
    std::cout << "ID матрицы: " << report.matrixId << "\n";
    std::cout << "Статус: " << complianceToString(report.compliancePercentage) << "\n";
    std::cout << "Компетенций выполнено: " << report.metCompetencies << "/" << report.totalCompetencies << "\n";
    std::cout << "Соответствие: ";
    printProgressBar(report.compliancePercentage);

    if (!report.gaps.empty()) {
        std::cout << "\nВыявленные пробелы: " << report.gaps.size() << "\n";
        std::cout << std::string(80, '-') << "\n";
        std::cout << std::left << std::setw(15) << "ID компетенции"
                  << std::setw(15) << "Требуется"
                  << std::setw(15) << "Фактически"
                  << std::setw(15) << "Пробел\n";
        std::cout << std::string(80, '-') << "\n";

        for (const auto& gap : report.gaps) {
            std::cout << std::left << std::setw(15) << gap.competenceId
                      << std::setw(15) << gap.requiredLevel
                      << std::setw(15) << gap.actualLevel
                      << std::setw(15) << gap.gap << "\n";
        }
    }
}
} // namespace Services
