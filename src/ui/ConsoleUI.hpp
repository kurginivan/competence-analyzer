#pragma once

#include <string>
#include <memory>
#include <iostream>
#include "../database/DatabaseManager.hpp"
#include "../services/AnalysisEngine.hpp"
#include "../services/ReportGenerator.hpp"

namespace UI {

class ConsoleUI {
public:
    ConsoleUI(std::shared_ptr<Database::DatabaseManager> db);
    ~ConsoleUI() = default;

    void run();

private:
    std::shared_ptr<Database::DatabaseManager> db;
    std::shared_ptr<Services::AnalysisEngine> analysisEngine;
    std::shared_ptr<Services::ReportGenerator> reportGenerator;

    // Main menu
    void displayMainMenu();
    void handleMainMenuChoice(int choice);

    // Employee management
    void displayEmployeeMenu();
    void addEmployee();
    void viewEmployee();
    void viewAllEmployees();
    void editEmployee();
    void deleteEmployee();

    // Competence management
    void displayCompetenceMenu();
    void addCompetence();
    void viewCompetence();
    void viewAllCompetences();
    void editCompetence();
    void deleteCompetence();

    // Matrix management
    void displayMatrixMenu();
    void addMatrix();
    void viewMatrix();
    void viewAllMatrices();
    void editMatrix();
    void manageMatrixCompetencies();
    void deleteMatrix();

    // Assessment management
    void displayAssessmentMenu();
    void assessEmployee();
    void viewEmployeeAssessments();
    void editAssessment();
    void deleteAssessment();

    // Analysis and reports
    void displayAnalysisMenu();
    void analyzeEmployeeCompliance();
    void analyzeMatrixCompliance();
    void viewGapAnalysis();
    void viewStatistics();

    // Utility methods
    void clearScreen();
    void pause();
    int getIntInput(const std::string& prompt);
    std::string getStringInput(const std::string& prompt);
    int getChoiceInput(int minChoice, int maxChoice);
};

} // namespace UI
