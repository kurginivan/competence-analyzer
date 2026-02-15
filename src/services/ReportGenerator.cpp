#include "ReportGenerator.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace Services {

ReportGenerator::ReportGenerator(std::shared_ptr<AnalysisEngine> engine)
    : engine(engine) {}

void ReportGenerator::printSeparator(int width) {
    std::cout << std::string(width, '=') << "\n";
}

void ReportGenerator::printHeader(const std::string& title, int width) {
    int padding = (width - title.length() - 2) / 2;
    std::cout << "\n";
    printSeparator(width);
    std::cout << std::string(padding, ' ') << " " << title << " " << "\n";
    printSeparator(width);
}

void ReportGenerator::printProgressBar(double percentage, int width) {
    int filled = static_cast<int>((percentage / 100.0) * width);
    std::cout << "[";
    for (int i = 0; i < width; i++) {
        if (i < filled) std::cout << "█";
        else std::cout << "░";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "%\n";
}

std::string ReportGenerator::levelToString(int level) {
    switch (level) {
        case 1: return "Базовый";
        case 2: return "Средний";
        case 3: return "Продвинутый";
        case 4: return "Эксперт";
        case 5: return "Мастер";
        default: return "Не оценено";
    }
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

void ReportGenerator::printMatrixComplianceReport(const std::vector<ComplianceReport>& reports) {
    if (reports.empty()) {
        std::cout << "Отчеты недоступны.\n";
        return;
    }

    int matrixId = reports[0].matrixId;
    printHeader("ОТЧЕТ О СООТВЕТСТВИИ МАТРИЦЫ", 100);

    std::cout << "\nID матрицы: " << matrixId << "\n";
    std::cout << std::string(100, '-') << "\n";
    std::cout << std::left << std::setw(15) << "ID сотрудника"
              << std::setw(30) << "Соответствие %"
              << std::setw(20) << "Статус"
              << std::setw(20) << "Пробелы\n";
    std::cout << std::string(100, '-') << "\n";

    for (const auto& report : reports) {
        std::cout << std::left << std::setw(15) << report.employeeId
                  << std::setw(12) << std::fixed << std::setprecision(1) << report.compliancePercentage
                  << std::setw(8) << "%"
                  << std::setw(20) << complianceToString(report.compliancePercentage)
                  << std::setw(20) << report.gaps.size() << "\n";
    }

    // Статистика по итогам
    double totalCompliance = 0;
    int fullyCompliant = 0;
    int totalGaps = 0;

    for (const auto& report : reports) {
        totalCompliance += report.compliancePercentage;
        if (report.compliancePercentage >= 100.0) fullyCompliant++;
        totalGaps += report.gaps.size();
    }

    std::cout << "\n" << std::string(100, '-') << "\n";
    std::cout << "СТАТИСТИКА ПО ИТОГАМ\n";
    std::cout << std::string(100, '-') << "\n";
    std::cout << "Всего сотрудников: " << reports.size() << "\n";
    std::cout << "Полностью соответствуют: " << fullyCompliant << " (" 
              << std::fixed << std::setprecision(1) 
              << (100.0 * fullyCompliant / reports.size()) << "%)\n";
    std::cout << "Среднее соответствие: " 
              << std::fixed << std::setprecision(1) 
              << (totalCompliance / reports.size()) << "%\n";
    std::cout << "Всего пробелов: " << totalGaps << "\n";
    printSeparator(100);
}

void ReportGenerator::printGapAnalysisReport(int employeeId, const std::vector<GapAnalysis>& gaps) {
    printHeader("ОТЧЕТ АНАЛИЗА ПРОБЕЛОВ", 80);

    std::cout << "ID сотрудника: " << employeeId << "\n";

    if (gaps.empty()) {
        std::cout << "\n✓ Пробелы не найдены. Сотрудник полностью соответствует требованиям!\n";
        printSeparator(80);
        return;
    }

    std::cout << "Всего пробелов: " << gaps.size() << "\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::left << std::setw(15) << "ID компетенции"
              << std::setw(15) << "Требуется"
              << std::setw(15) << "Фактически"
              << std::setw(15) << "Пробел\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& gap : gaps) {
        std::cout << std::left << std::setw(15) << gap.competenceId
                  << std::setw(15) << levelToString(gap.requiredLevel)
                  << std::setw(15) << levelToString(gap.actualLevel)
                  << std::setw(15) << gap.gap << " уровней\n";
    }

    printSeparator(80);
}

void ReportGenerator::printMatrixGapAnalysisReport(int matrixId, const std::map<int, std::vector<GapAnalysis>>& allGaps) {
    printHeader("ОТЧЕТ АНАЛИЗА ПРОБЕЛОВ МАТРИЦЫ", 100);

    std::cout << "ID матрицы: " << matrixId << "\n";

    if (allGaps.empty()) {
        std::cout << "Пробелы не найдены!\n";
        printSeparator(100);
        return;
    }

    for (const auto& [employeeId, gaps] : allGaps) {
        std::cout << "\nID сотрудника: " << employeeId << " - Пробелов: " << gaps.size() << "\n";
        for (const auto& gap : gaps) {
            std::cout << "  • Компетенция " << gap.competenceId
                      << ": Требуется " << gap.requiredLevel
                      << ", Фактически " << gap.actualLevel
                      << " (Пробел: " << gap.gap << ")\n";
        }
    }

    printSeparator(100);
}

void ReportGenerator::printMatrixStatistics(int matrixId) {
    printHeader("СТАТИСТИКА МАТРИЦЫ", 80);

    try {
        double avgCompliance = engine->getAverageComplianceForMatrix(matrixId);
        int competentCount = engine->getCompetentEmployeeCount(matrixId);

        std::cout << "\nСреднее соответствие: " 
                  << std::fixed << std::setprecision(1) << avgCompliance << "%\n";
        std::cout << "Сотрудники полностью соответствуют: " << competentCount << "\n";

        auto averageLevels = engine->getCompetenceAverageLevel(matrixId);
        if (!averageLevels.empty()) {
            std::cout << "\nСредние уровни компетенций:\n";
            std::cout << std::string(80, '-') << "\n";
            for (const auto& [competenceId, avgLevel] : averageLevels) {
                std::cout << "Компетенция " << std::setw(5) << competenceId 
                          << ": " << std::fixed << std::setprecision(2) << avgLevel
                          << "/5 (" << levelToString(static_cast<int>(std::round(avgLevel))) << ")\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при создании статистики матрицы: " << e.what() << "\n";
    }

    printSeparator(80);
}

void ReportGenerator::printCompetenceStatistics(int matrixId) {
    printHeader("СТАТИСТИКА КОМПЕТЕНЦИЙ ДЛЯ МАТРИЦЫ", 80);

    try {
        auto averageLevels = engine->getCompetenceAverageLevel(matrixId);

        if (averageLevels.empty()) {
            std::cout << "Данные по компетенциям недоступны.\n";
        } else {
            std::cout << std::left << std::setw(20) << "ID компетенции"
                      << std::setw(20) << "Средний уровень"
                      << std::setw(30) << "Визуализация\n";
            std::cout << std::string(80, '-') << "\n";

            for (const auto& [competenceId, avgLevel] : averageLevels) {
                std::cout << std::left << std::setw(20) << competenceId
                          << std::setw(20) << std::fixed << std::setprecision(2) << avgLevel;
                printProgressBar(avgLevel * 20, 30); // Преобразуем 0-5 в 0-100
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при создании статистики компетенций: " << e.what() << "\n";
    }

    printSeparator(80);
}

void ReportGenerator::printEmployeeStatistics(int employeeId) {
    printHeader("СТАТИСТИКА СОТРУДНИКА", 80);

    try {
        double avgLevel = engine->getEmployeeAverageLevel(employeeId);

        std::cout << "ID сотрудника: " << employeeId << "\n";
        std::cout << "Средний уровень компетенции: " << std::fixed << std::setprecision(2) << avgLevel << "/5\n";
        std::cout << "Уровень: " << levelToString(static_cast<int>(std::round(avgLevel))) << "\n";

        auto competenceMap = engine->getEmployeeCompetenceMap(employeeId);
        if (!competenceMap.empty()) {
            std::cout << "\nРазбор компетенций:\n";
            std::cout << std::string(80, '-') << "\n";
            std::cout << std::left << std::setw(20) << "ID компетенции"
                      << std::setw(20) << "Уровень"
                      << std::setw(30) << "Прогресс\n";
            std::cout << std::string(80, '-') << "\n";

            for (const auto& [competenceId, level] : competenceMap) {
                std::cout << std::left << std::setw(20) << competenceId
                          << std::setw(20) << levelToString(static_cast<int>(level));
                printProgressBar(level * 20, 30); // Преобразуем 0-5 в 0-100
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при создании статистики сотрудника: " << e.what() << "\n";
    }

    printSeparator(80);
}

void ReportGenerator::printDetailedComplianceAnalysis(int employeeId, int matrixId) {
    printHeader("ДЕТАЛЬНЫЙ АНАЛИЗ СООТВЕТСТВИЯ", 100);

    try {
        auto report = engine->analyzeEmployeeCompliance(employeeId, matrixId);
        printEmployeeComplianceReport(report);

        // Дополнительная информация о сотруднике
        std::cout << "\n" << std::string(100, '-') << "\n";
        std::cout << "ОБЩАЯ ИНФОРМАЦИЯ О СОТРУДНИКЕ\n";
        std::cout << std::string(100, '-') << "\n";
        double avgLevel = engine->getEmployeeAverageLevel(employeeId);
        std::cout << "Общий средний уровень: " << std::fixed << std::setprecision(2) << avgLevel << "/5\n";

    } catch (const std::exception& e) {
        std::cerr << "Ошибка при создании детального анализа: " << e.what() << "\n";
    }

    printSeparator(100);
}

void ReportGenerator::printMatrixOverallAnalysis(int matrixId) {
    auto reports = engine->analyzeAllEmployeeCompliance(matrixId);
    printMatrixComplianceReport(reports);
    printMatrixStatistics(matrixId);
}

std::string ReportGenerator::generateComplianceReportText(const ComplianceReport& report) {
    std::ostringstream oss;
    oss << "Отчет о соответствии\n";
    oss << "ID сотрудника: " << report.employeeId << "\n";
    oss << "ID матрицы: " << report.matrixId << "\n";
    oss << "Соответствие: " << std::fixed << std::setprecision(1) << report.compliancePercentage << "%\n";
    oss << "Выполнено: " << report.metCompetencies << "/" << report.totalCompetencies << "\n";
    return oss.str();
}

std::string ReportGenerator::generateGapAnalysisText(int employeeId, const std::vector<GapAnalysis>& gaps) {
    std::ostringstream oss;
    oss << "Анализ пробелов для сотрудника " << employeeId << "\n";
    oss << "Всего пробелов: " << gaps.size() << "\n";
    for (const auto& gap : gaps) {
        oss << "Компетенция " << gap.competenceId << ": Пробел " << gap.gap << " уровней\n";
    }
    return oss.str();
}

} // namespace Services
