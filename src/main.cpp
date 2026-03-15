#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include "database/DatabaseManager.hpp"
#include "ui/ConsoleUI.hpp"

int main() {
    try {
        // Read environment variables for database connection
        const char* dbHost = std::getenv("DATABASE_HOST");
        const char* dbPort = std::getenv("DATABASE_PORT");
        const char* dbUser = std::getenv("DATABASE_USER");
        const char* dbPassword = std::getenv("DATABASE_PASSWORD");
        const char* dbName = std::getenv("DATABASE_NAME");

        std::string host = dbHost ? dbHost : "localhost";
        std::string port = dbPort ? dbPort : "5432";
        std::string user = dbUser ? dbUser : "postgres";
        std::string password = dbPassword ? dbPassword : "postgres";
        std::string dbname = dbName ? dbName : "competence_analyzer";

        std::string connStr = "user=" + user + 
                             " password=" + password + 
                             " host=" + host + 
                             " port=" + port + 
                             " dbname=" + dbname +
                             " connect_timeout=10";

        std::cout << "Подключение к базе данных на " << host << ":" << port << "...\n";

        auto dbManager = std::make_shared<Database::DatabaseManager>(connStr);

        if (!dbManager->connect()) {
            std::cerr << "Ошибка подключения к базе данных. Убедитесь, что PostgreSQL запущен.\n";
            std::cerr << "Строка подключения: " << connStr << "\n";
            return 1;
        }

        auto ui = std::make_unique<UI::ConsoleUI>(dbManager);
        ui->run();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << "\n";
        return 1;
    }
}
