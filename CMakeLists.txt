cmake_minimum_required(VERSION 3.20)
project(CompetenceAnalyzer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx libpq)
pkg_check_modules(PQ REQUIRED libpq)

# PostgreSQL
find_package(PostgreSQL REQUIRED)

# Debug: Print found packages info
message(STATUS "PostgreSQL found: ${PostgreSQL_FOUND}")
message(STATUS "PostgreSQL include: ${PostgreSQL_INCLUDE_DIRS}")
message(STATUS "PostgreSQL libs: ${PostgreSQL_LIBRARIES}")
message(STATUS "PQXX libs: ${PQXX_LIBRARIES}")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PostgreSQL_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIRS}
    ${PQ_INCLUDE_DIRS}
)

# Add library directories
link_directories(
    ${PostgreSQL_LIBRARY_DIRS}
    ${PQXX_LIBRARY_DIRS}
    ${PQ_LIBRARY_DIRS}
)

# Also add pkg-config directories
if(PQXX_LIBRARY_DIRS)
    link_directories(${PQXX_LIBRARY_DIRS})
endif()
if(PQ_LIBRARY_DIRS)
    link_directories(${PQ_LIBRARY_DIRS})
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/database/DatabaseManager.cpp
    src/models/Employee.cpp
    src/models/Competence.cpp
    src/models/Position.cpp
    src/models/Matrix.cpp
    src/models/Assessment.cpp
    src/services/AnalysisEngine.cpp
    src/services/ReportGenerator.cpp
    src/ui/ConsoleUI.cpp
    src/utils/helpers.cpp
)

# Qt sources
set(QT_SOURCES
    src/ui/MainWindow.hpp
    src/ui/MainWindow.cpp
    src/qt_main.cpp
    src/database/DatabaseManager.cpp
    src/models/Employee.cpp
    src/models/Competence.cpp
    src/models/Position.cpp
    src/models/Matrix.cpp
    src/models/Assessment.cpp
    src/services/AnalysisEngine.cpp
    src/services/ReportGenerator.cpp
    src/utils/helpers.cpp
)

# Create console executable
add_executable(competence-analyzer ${SOURCES})

# Link libraries for console version
target_link_libraries(competence-analyzer
    PRIVATE
    pqxx
    pq
    stdc++
)

# Add compile options and include directories
target_include_directories(competence-analyzer PRIVATE ${PQXX_INCLUDE_DIRS} ${PQ_INCLUDE_DIRS})
target_compile_options(competence-analyzer PRIVATE ${PQXX_CFLAGS_OTHER} ${PQ_CFLAGS_OTHER})
target_link_directories(competence-analyzer PRIVATE ${PQXX_LIBRARY_DIRS} ${PQ_LIBRARY_DIRS})

# Create install targets
install(TARGETS competence-analyzer DESTINATION bin)
install(FILES sql/schema.sql DESTINATION share/competence-analyzer)
install(FILES sql/test_data.sql DESTINATION share/competence-analyzer)

