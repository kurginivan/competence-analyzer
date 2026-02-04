#!/bin/bash

# Quick start and environment checker for Competence Analyzer

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║  Competence Analyzer - Environment Check & Quick Start    ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Keep track of what's available
HAS_DOCKER=false
HAS_CMAKE=false
HAS_QT5=false
HAS_COMPILER=false
HAS_POSTGRESQL=false

echo -e "${BLUE}Проверка окружения...${NC}"
echo ""

# Check Docker
echo -n "Docker:          "
if command -v docker &> /dev/null && command -v docker-compose &> /dev/null; then
    echo -e "${GREEN}✓ Установлен${NC}"
    HAS_DOCKER=true
else
    echo -e "${RED}✗ Не установлен${NC}"
fi

# Check PostgreSQL
echo -n "PostgreSQL:      "
if command -v psql &> /dev/null; then
    PSQL_VERSION=$(psql --version | cut -d ' ' -f 3)
    if pg_isready -h localhost -p 5432 > /dev/null 2>&1; then
        echo -e "${GREEN}✓ Установлен и запущен (v$PSQL_VERSION)${NC}"
        HAS_POSTGRESQL=true
    else
        echo -e "${YELLOW}⚠ Установлен (v$PSQL_VERSION), но не запущен${NC}"
    fi
else
    echo -e "${RED}✗ Не установлен${NC}"
fi

# Check CMake
echo -n "CMake:           "
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -1 | cut -d ' ' -f 3)
    echo -e "${GREEN}✓ Установлен (v$CMAKE_VERSION)${NC}"
    HAS_CMAKE=true
else
    echo -e "${RED}✗ Не установлен${NC}"
fi

# Check Qt5
echo -n "Qt5:             "
if command -v qmake &> /dev/null; then
    QT_VERSION=$(qmake --version | grep "Using Qt" | cut -d ' ' -f 4)
    echo -e "${GREEN}✓ Установлен (v$QT_VERSION)${NC}"
    HAS_QT5=true
else
    echo -e "${RED}✗ Не установлен${NC}"
fi

# Check Compiler
echo -n "G++:             "
if command -v g++ &> /dev/null; then
    GCC_VERSION=$(g++ --version | head -1 | grep -oP '\d+\.\d+')
    echo -e "${GREEN}✓ Установлен (v$GCC_VERSION)${NC}"
    HAS_COMPILER=true
else
    echo -e "${RED}✗ Не установлен${NC}"
fi

echo ""
echo -e "${BLUE}Рекомендации:${NC}"
echo ""

# Provide recommendations
if [ "$HAS_DOCKER" = true ]; then
    echo -e "${GREEN}[1]${NC} Docker доступен - рекомендуется для консольной версии:"
    echo "    cd $PROJECT_DIR && docker-compose up -d"
    echo "    docker exec -it competence_analyzer_app /app/build/competence-analyzer"
    echo ""
fi

if [ "$HAS_POSTGRESQL" = true ] && [ "$HAS_CMAKE" = true ] && [ "$HAS_QT5" = true ] && [ "$HAS_COMPILER" = true ]; then
    echo -e "${GREEN}[2]${NC} Qt GUI может быть собран и запущен локально:"
    echo "    cd $PROJECT_DIR && ./run-qt.sh"
    echo ""
fi

if [ "$HAS_POSTGRESQL" = false ] && [ "$HAS_DOCKER" = false ]; then
    echo -e "${RED}[!]${NC} PostgreSQL не запущен и Docker не доступен"
    echo "    Решения:"
    echo "    • Запустить PostgreSQL локально: sudo service postgresql start"
    echo "    • Использовать Docker: docker-compose up -d"
    echo ""
fi

if [ "$HAS_CMAKE" = false ] || [ "$HAS_QT5" = false ] || [ "$HAS_COMPILER" = false ]; then
    echo -e "${YELLOW}[!]${NC} Для Qt GUI требуются зависимости:"
    echo "    sudo apt-get install -y build-essential cmake qt5-qmake qtbase5-dev qt5-default"
    echo ""
fi

echo -e "${BLUE}Быстрый старт:${NC}"
echo ""

# Recommend best option
if [ "$HAS_DOCKER" = true ]; then
    echo -e "${GREEN}1. Консольная версия в Docker (самый простой способ):${NC}"
    echo "   cd $PROJECT_DIR"
    echo "   docker-compose up -d"
    echo "   docker exec -it competence_analyzer_app /app/build/competence-analyzer"
    echo ""
fi

if [ "$HAS_QT5" = true ] && [ "$HAS_CMAKE" = true ] && [ "$HAS_COMPILER" = true ]; then
    echo -e "${GREEN}2. Qt GUI локально (если PostgreSQL запущен):${NC}"
    echo "   cd $PROJECT_DIR"
    echo "   ./run-qt.sh"
    echo ""
fi

if [ "$HAS_DOCKER" = false ] && [ "$HAS_POSTGRESQL" = false ]; then
    echo -e "${YELLOW}Сначала установите PostgreSQL или используйте Docker:${NC}"
    echo ""
    echo -e "${BLUE}Вариант А (рекомендуется - Docker):${NC}"
    echo "   sudo apt-get install docker.io docker-compose"
    echo "   sudo usermod -aG docker \$USER"
    echo "   # Перезагрузитесь, затем:"
    echo "   docker-compose up -d"
    echo ""
    echo -e "${BLUE}Вариант Б (локальный PostgreSQL):${NC}"
    echo "   sudo apt-get install postgresql postgresql-client libpq-dev"
    echo "   sudo service postgresql start"
    echo ""
fi

# Final summary
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"
echo ""

if [ "$HAS_DOCKER" = false ] && [ "$HAS_POSTGRESQL" = false ] && [ "$HAS_CMAKE" = false ]; then
    echo -e "${RED}Требуется базовая установка. Смотрите SETUP_GUIDE.md для полных инструкций.${NC}"
    echo ""
    echo "cd $PROJECT_DIR && cat SETUP_GUIDE.md"
else
    echo -e "${GREEN}Окружение частично настроено. Выберите подходящий вариант запуска выше.${NC}"
fi

echo ""
