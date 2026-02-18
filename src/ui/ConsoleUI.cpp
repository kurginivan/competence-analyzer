#include "ConsoleUI.hpp"
#include <iostream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <unordered_map>

namespace UI {

ConsoleUI::ConsoleUI(std::shared_ptr<Database::DatabaseManager> db)
    : db(db) {
    analysisEngine = std::make_shared<Services::AnalysisEngine>(db);
    reportGenerator = std::make_shared<Services::ReportGenerator>(analysisEngine);
}

void ConsoleUI::run() {
    int choice = 0;

    while (true) {
        displayMainMenu();
        
        std::cout.flush();
        choice = getChoiceInput(0, 5);
        
        if (choice == 0) {
            break;
        }
        
        handleMainMenuChoice(choice);
    }

    std::cout << "\nСпасибо за использование системы анализа матриц компетенций. До свидания!\n";
}

void ConsoleUI::displayMainMenu() {
    clearScreen();
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║   Система анализа матриц компетенций           ║\n";
    std::cout << "║   v1.0.0                                       ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n\n";

    std::cout << "Главное меню:\n";
    std::cout << "1. Управление сотрудниками\n";
    std::cout << "2. Управление компетенциями\n";
    std::cout << "3. Управление матрицами\n";
    std::cout << "4. Управление оценками\n";
    std::cout << "5. Анализ и отчеты\n";
    std::cout << "0. Выход\n";
    std::cout << "\nВыберите опцию (0-5): ";
}

void ConsoleUI::handleMainMenuChoice(int choice) {
    switch (choice) {
        case 1: displayEmployeeMenu(); break;
        case 2: displayCompetenceMenu(); break;
        case 3: displayMatrixMenu(); break;
        case 4: displayAssessmentMenu(); break;
        case 5: displayAnalysisMenu(); break;
        default:
            std::cout << "Неверный выбор. Пожалуйста, попробуйте еще раз.\n";
            pause();
    }
}

// ==================== Управление сотрудниками ====================

void ConsoleUI::displayEmployeeMenu() {
    clearScreen();
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n=== Управление сотрудниками ===\n";
        std::cout << "1. Добавить сотрудника\n";
        std::cout << "2. Просмотреть всех сотрудников\n";
        std::cout << "3. Удалить сотрудника\n";
        std::cout << "0. Вернуться в главное меню\n";
        std::cout << "\nВыберите опцию (0-3): ";

        choice = getChoiceInput(0, 3);

        switch (choice) {
            case 1: addEmployee(); break;
            case 2: viewAllEmployees(); break;
            case 3: deleteEmployee(); break;
            case 0: break;
            default:
                std::cout << "Неверный выбор.\n";
        }
    }
}

void ConsoleUI::addEmployee() {
    std::cout << "\n=== Добавление нового сотрудника ===\n";

    std::string name = getStringInput("Имя сотрудника: ");
    int positionId = -1;
    try {
        auto positions = db->getAllPositions();
        if (!positions.empty()) {
            std::cout << "\nДоступные должности:\n";
            std::cout << std::string(60, '-') << "\n";
            std::cout << std::left << std::setw(10) << "ID"
                      << std::setw(40) << "Название" << "\n";
            std::cout << std::string(60, '-') << "\n";
            for (const auto& pos : positions) {
                std::cout << std::left << std::setw(10) << pos.getId()
                          << std::setw(40) << pos.getName() << "\n";
            }
            std::cout << std::string(60, '-') << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Ошибка при загрузке должностей: " << e.what() << "\n";
    }
    int inputPositionId = getIntInput("ID должности (0 - без должности): ");
    if (inputPositionId > 0) {
        positionId = inputPositionId;
    }
    std::string department = getStringInput("Отдел: ");
    std::string email = getStringInput("Email: ");

    try {
        Models::Employee emp(0, name, positionId, department, email);
        int id = db->addEmployee(emp);
        std::cout << "\n✓ Сотрудник успешно добавлен! ID: " << id << "\n";
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::viewAllEmployees() {
    try {
        auto employees = db->getAllEmployees();
        std::unordered_map<int, std::string> positionNames;
        try {
            auto positions = db->getAllPositions();
            for (const auto& pos : positions) {
                positionNames[pos.getId()] = pos.getName();
            }
        } catch (const std::exception&) {
            positionNames.clear();
        }

        std::cout << "\n=== Все сотрудники ===\n";
        if (employees.empty()) {
            std::cout << "Сотрудники не найдены.\n";
        } else {
            std::cout << std::string(100, '-') << "\n";
            std::cout << std::left << std::setw(10) << "ID"
                      << std::setw(25) << "Имя"
                      << std::setw(20) << "Должность"
                      << std::setw(25) << "Отдел"
                      << std::setw(20) << "Email\n";
            std::cout << std::string(100, '-') << "\n";

            for (const auto& emp : employees) {
                std::string positionName = "-";
                if (emp.getPositionId() != -1) {
                    auto it = positionNames.find(emp.getPositionId());
                    if (it != positionNames.end()) {
                        positionName = it->second;
                    } else {
                        positionName = std::to_string(emp.getPositionId());
                    }
                }
                std::cout << std::left << std::setw(10) << emp.getId()
                          << std::setw(25) << emp.getName()
                          << std::setw(20) << positionName
                          << std::setw(25) << emp.getDepartment()
                          << std::setw(20) << emp.getEmail() << "\n";
            }
            std::cout << std::string(100, '-') << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::deleteEmployee() {
    int id = getIntInput("\nВведите ID сотрудника для удаления: ");

    if (db->deleteEmployee(id)) {
        std::cout << "\n✓ Сотрудник успешно удален!\n";
    } else {
        std::cout << "\n✗ Ошибка при удалении сотрудника.\n";
    }

    pause();
}

// ==================== Управление компетенциями ====================

void ConsoleUI::displayCompetenceMenu() {
    clearScreen();
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n=== Управление компетенциями ===\n";
        std::cout << "1. Добавить компетенцию\n";
        std::cout << "2. Просмотреть все компетенции\n";
        std::cout << "3. Удалить компетенцию\n";
        std::cout << "0. Вернуться в главное меню\n";
        std::cout << "\nВыберите опцию (0-3): ";

        choice = getChoiceInput(0, 3);

        switch (choice) {
            case 1: addCompetence(); break;
            case 2: viewAllCompetences(); break;
            case 3: deleteCompetence(); break;
            case 0: break;
            default:
                std::cout << "Неверный выбор.\n";
        }
    }
}

void ConsoleUI::addCompetence() {
    std::cout << "\n=== Добавление новой компетенции ===\n";

    std::string name = getStringInput("Название компетенции: ");
    std::string description = getStringInput("Описание: ");
    std::string category = getStringInput("Категория (например, Криптография, Сетевая безопасность): ");

    try {
        Models::Competence comp(0, name, description, category);
        int id = db->addCompetence(comp);
        std::cout << "\n✓ Компетенция успешно добавлена! ID: " << id << "\n";
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::viewAllCompetences() {
    try {
        auto competences = db->getAllCompetences();

        std::cout << "\n=== Все компетенции ===\n";
        if (competences.empty()) {
            std::cout << "Компетенции не найдены.\n";
        } else {
            std::cout << std::string(120, '-') << "\n";
            std::cout << std::left << std::setw(10) << "ID"
                      << std::setw(25) << "Название"
                      << std::setw(30) << "Категория"
                      << std::setw(55) << "Описание\n";
            std::cout << std::string(120, '-') << "\n";

            for (const auto& comp : competences) {
                std::string desc = comp.getDescription();
                if (desc.length() > 50) desc = desc.substr(0, 50) + "...";
                std::cout << std::left << std::setw(10) << comp.getId()
                          << std::setw(25) << comp.getName()
                          << std::setw(30) << comp.getCategory()
                          << std::setw(55) << desc << "\n";
            }
            std::cout << std::string(120, '-') << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::deleteCompetence() {
    int id = getIntInput("\nВведите ID компетенции для удаления: ");

    if (db->deleteCompetence(id)) {
        std::cout << "\n✓ Компетенция успешно удалена!\n";
    } else {
        std::cout << "\n✗ Ошибка при удалении компетенции.\n";
    }

    pause();
}

// ==================== Управление матрицами ====================

void ConsoleUI::displayMatrixMenu() {
    clearScreen();
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n=== Управление матрицами ===\n";
        std::cout << "1. Добавить матрицу\n";
        std::cout << "2. Просмотреть матрицу\n";
        std::cout << "3. Просмотреть все матрицы\n";
        std::cout << "4. Управлять компетенциями матрицы\n";
        std::cout << "5. Удалить матрицу\n";
        std::cout << "0. Вернуться в главное меню\n";
        std::cout << "\nВыберите опцию (0-5): ";

        choice = getChoiceInput(0, 5);

        switch (choice) {
            case 1: addMatrix(); break;
            case 2: viewMatrix(); break;
            case 3: viewAllMatrices(); break;
            case 4: manageMatrixCompetencies(); break;
            case 5: deleteMatrix(); break;
            case 0: break;
            default:
                std::cout << "Неверный выбор.\n";
        }
    }
}

void ConsoleUI::addMatrix() {
    std::cout << "\n=== Добавление новой матрицы ===\n";

    std::string name = getStringInput("Название матрицы: ");
    std::string description = getStringInput("Описание: ");

    try {
        Models::Matrix matrix(0, name, description);
        int matrixId = db->addMatrix(matrix);
        std::cout << "\n✓ Матрица успешно добавлена! ID: " << matrixId << "\n";

        // Предложить связать должности с матрицей
        std::cout << "\nХотите связать должности с этой матрицей?\n";
        std::cout << "1. Добавить новую должность для этой матрицы\n";
        std::cout << "2. Связать существующую должность с матрицей\n";
        std::cout << "3. Пропустить\n";
        int choice = getChoiceInput(1, 3);

        if (choice == 1) {
            // Создать новую должность
            std::string positionName = getStringInput("Название должности: ");
            Models::Position position(0, positionName, matrixId);
            int positionId = db->addPosition(position);
            std::cout << "\n✓ Должность успешно создана и связана с матрицей! ID должности: " << positionId << "\n";
        } else if (choice == 2) {
            // Показать существующие должности
            auto positions = db->getAllPositions();
            if (positions.empty()) {
                std::cout << "\nЭкземпляры должности отсутствуют.\n";
            } else {
                std::cout << "\n=== Существующие должности ===\n";
                std::cout << std::string(60, '-') << "\n";
                std::cout << std::left << std::setw(10) << "ID"
                          << std::setw(30) << "Название"
                          << std::setw(20) << "Текущая матрица\n";
                std::cout << std::string(60, '-') << "\n";

                for (const auto& pos : positions) {
                    std::string matrixInfo = (pos.getMatrixId() == -1) ? "не указана" : std::to_string(pos.getMatrixId());
                    std::cout << std::left << std::setw(10) << pos.getId()
                              << std::setw(30) << pos.getName()
                              << std::setw(20) << matrixInfo << "\n";
                }
                std::cout << std::string(60, '-') << "\n";

                int positionId = getIntInput("ID должности для связи: ");
                Models::Position position = db->getPosition(positionId);
                position.setMatrixId(matrixId);
                if (db->updatePosition(position)) {
                    std::cout << "\n✓ Должность успешно связана с матрицей!\n";
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::viewMatrix() {
    int id = getIntInput("\nВведите ID матрицы: ");

    try {
        auto matrix = db->getMatrix(id);
        std::cout << "\n=== Информация о матрице ===\n";
        matrix.display();

        auto competencies = db->getMatrixCompetencies(id);
        if (!competencies.empty()) {
            std::cout << "\nСвязанные компетенции:\n";
            std::cout << std::string(60, '-') << "\n";
            std::cout << std::left << std::setw(20) << "ID Компетенции"
                      << std::setw(20) << "Требуемый уровень\n";
            std::cout << std::string(60, '-') << "\n";

            for (const auto& [competenceId, requiredLevel] : competencies) {
                std::cout << std::left << std::setw(20) << competenceId
                          << std::setw(20) << requiredLevel << "\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::viewAllMatrices() {
    try {
        auto matrices = db->getAllMatrices();

        std::cout << "\n=== Все матрицы ===\n";
        if (matrices.empty()) {
            std::cout << "Матрицы не найдены.\n";
        } else {
            std::cout << std::string(100, '-') << "\n";
            std::cout << std::left << std::setw(10) << "ID"
                      << std::setw(30) << "Название"
                      << std::setw(60) << "Описание\n";
            std::cout << std::string(100, '-') << "\n";

            for (const auto& matrix : matrices) {
                std::string desc = matrix.getDescription();
                if (desc.length() > 55) desc = desc.substr(0, 55) + "...";
                std::cout << std::left << std::setw(10) << matrix.getId()
                          << std::setw(30) << matrix.getName()
                          << std::setw(60) << desc << "\n";
            }
            std::cout << std::string(100, '-') << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::manageMatrixCompetencies() {
    int matrixId = getIntInput("\nВведите ID матрицы: ");

    try {
        auto matrix = db->getMatrix(matrixId);

        std::cout << "\nТекущие компетенции:\n";
        auto competencies = db->getMatrixCompetencies(matrixId);
        if (competencies.empty()) {
            std::cout << "Компетенции не связаны с этой матрицей.\n";
        } else {
            for (const auto& [compId, level] : competencies) {
                std::cout << "  Компетенция " << compId << ": Уровень " << level << "\n";
            }
        }

        std::cout << "\n1. Добавить компетенцию\n";
        std::cout << "2. Удалить компетенцию\n";
        std::cout << "0. Назад\n";
        int choice = getChoiceInput(0, 2);

        switch (choice) {
            case 1: {
                int competenceId = getIntInput("ID компетенции: ");
                int requiredLevel = getIntInput("Требуемый уровень (1-5): ");
                if (db->addCompetencyToMatrix(matrixId, competenceId, requiredLevel)) {
                    std::cout << "✓ Компетенция добавлена в матрицу.\n";
                }
                break;
            }
            case 2: {
                int competenceId = getIntInput("ID компетенции для удаления: ");
                if (db->removeCompetencyFromMatrix(matrixId, competenceId)) {
                    std::cout << "✓ Компетенция удалена из матрицы.\n";
                }
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::deleteMatrix() {
    int id = getIntInput("\nВведите ID матрицы для удаления: ");

    if (db->deleteMatrix(id)) {
        std::cout << "\n✓ Матрица успешно удалена!\n";
    } else {
        std::cout << "\n✗ Ошибка при удалении матрицы.\n";
    }

    pause();
}

// ==================== Управление оценками ====================

void ConsoleUI::displayAssessmentMenu() {
    clearScreen();
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n=== Управление оценками ===\n";
        std::cout << "1. Оценить компетенцию сотрудника\n";
        std::cout << "2. Просмотреть оценки сотрудника\n";
        std::cout << "3. Удалить оценку\n";
        std::cout << "0. Вернуться в главное меню\n";
        std::cout << "\nВыберите опцию (0-3): ";

        choice = getChoiceInput(0, 3);

        switch (choice) {
            case 1: assessEmployee(); break;
            case 2: viewEmployeeAssessments(); break;
            case 3: deleteAssessment(); break;
            case 0: break;
            default:
                std::cout << "Неверный выбор.\n";
        }
    }
}

void ConsoleUI::assessEmployee() {
    std::cout << "\n=== Оценка компетенции сотрудника ===\n";

    int employeeId = getIntInput("ID сотрудника: ");
    int competenceId = getIntInput("ID компетенции: ");
    int level = getIntInput("Уровень компетенции (1-5): ");

    if (level < 1 || level > 5) {
        std::cout << "✗ Уровень должен быть от 1 до 5.\n";
        pause();
        return;
    }

    std::string comments = getStringInput("Комментарии (опционально): ");

    try {
        Models::Assessment assessment(0, employeeId, competenceId, level, comments);
        int id = db->addAssessment(assessment);
        std::cout << "\n✓ Оценка успешно записана! ID: " << id << "\n";
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::viewEmployeeAssessments() {
    int employeeId = getIntInput("\nВведите ID сотрудника: ");

    try {
        auto assessments = db->getEmployeeAssessments(employeeId);

        std::cout << "\n=== Оценки для сотрудника " << employeeId << " ===\n";
        if (assessments.empty()) {
            std::cout << "Оценки для этого сотрудника не найдены.\n";
        } else {
            std::cout << std::string(80, '-') << "\n";
            std::cout << std::left << std::setw(10) << "ID"
                      << std::setw(15) << "Компетенция"
                      << std::setw(15) << "Уровень"
                      << std::setw(40) << "Комментарии\n";
            std::cout << std::string(80, '-') << "\n";

            for (const auto& assess : assessments) {
                std::string comments = assess.getComments();
                if (comments.length() > 35) comments = comments.substr(0, 35) + "...";
                std::cout << std::left << std::setw(10) << assess.getId()
                          << std::setw(15) << assess.getCompetenceId()
                          << std::setw(15) << assess.getActualLevel()
                          << std::setw(40) << comments << "\n";
            }
            std::cout << std::string(80, '-') << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

void ConsoleUI::deleteAssessment() {
    int id = getIntInput("\nВведите ID оценки для удаления: ");

    if (db->deleteAssessment(id)) {
        std::cout << "\n✓ Оценка успешно удалена!\n";
    } else {
        std::cout << "\n✗ Ошибка при удалении оценки.\n";
    }

    pause();
}

// ==================== Анализ и отчеты ====================

void ConsoleUI::displayAnalysisMenu() {
    clearScreen();
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n=== Анализ и отчеты ===\n";
        std::cout << "1. Анализ соответствия сотрудника\n";
        std::cout << "0. Вернуться в главное меню\n";
        std::cout << "\nВыберите опцию (0-1): ";

        choice = getChoiceInput(0, 1);

        switch (choice) {
            case 1: analyzeEmployeeCompliance(); break;
            case 0: break;
            default:
                std::cout << "Неверный выбор.\n";
        }
    }
}

void ConsoleUI::analyzeEmployeeCompliance() {
    int employeeId = getIntInput("\nВведите ID сотрудника: ");

    try {
        auto employee = db->getEmployee(employeeId);
        
        int matrixId = -1;
        if (employee.getPositionId() != -1) {
            auto position = db->getPosition(employee.getPositionId());
            if (position.getMatrixId() != -1) {
                matrixId = position.getMatrixId();
                std::cout << "\n✓ Используем матрицу для должности (ID матрицы: " << matrixId << ")\n";
            }
        }

        if (matrixId == -1) {
            auto allMatrices = db->getAllMatrices();
            std::cout << "\nМатрица для сотрудника не задана. Выберите матрицу для анализа:\n";
            std::cout << std::string(60, '-') << "\n";
            std::cout << std::left << std::setw(10) << "ID" << std::setw(50) << "Название\n";
            std::cout << std::string(60, '-') << "\n";
            for (const auto& matrix : allMatrices) {
                std::cout << std::left << std::setw(10) << matrix.getId()
                          << std::setw(50) << matrix.getName() << "\n";
            }
            std::cout << std::string(60, '-') << "\n";
            matrixId = getIntInput("ID матрицы: ");
        }

        auto report = analysisEngine->analyzeEmployeeCompliance(employeeId, matrixId);
        reportGenerator->printEmployeeComplianceReport(report);
    } catch (const std::exception& e) {
        std::cerr << "✗ Ошибка: " << e.what() << "\n";
    }

    pause();
}

// ==================== Вспомогательные методы ====================

void ConsoleUI::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
        // Fallback if clear doesn't work
        std::cout.flush();
    #endif
}

void ConsoleUI::pause() {
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int ConsoleUI::getIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Неверный ввод. Пожалуйста, введите число.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string ConsoleUI::getStringInput(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

int ConsoleUI::getChoiceInput(int minChoice, int maxChoice) {
    int choice = -1;
    
    while (true) {
        if (std::cin >> choice) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            if (choice >= minChoice && choice <= maxChoice) {
                return choice;
            }
            
            std::cout << "\n❌ Неверный выбор! Пожалуйста, введите число от " 
                      << minChoice << " до " << maxChoice << ": ";
            choice = -1;
        } else {
            std::cout << "\n❌ Неверный ввод! Пожалуйста, введите корректное число: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

} // namespace UI
