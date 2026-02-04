#!/bin/bash

# ============================================================
# Installation Script for Local Setup
# ============================================================

set -e

echo "=========================================="
echo "Competence Analyzer - Installation Script"
echo "=========================================="
echo ""

# Check if running on Ubuntu/Debian
if ! command -v apt-get &> /dev/null; then
    echo "✗ This script is designed for Ubuntu/Debian systems."
    echo "  For other systems, please install dependencies manually:"
    echo "  - PostgreSQL 15+"
    echo "  - CMake 3.20+"
    echo "  - libpqxx-dev"
    echo "  - build-essential"
    exit 1
fi

echo "Installing dependencies..."

# Update package lists
echo "Updating package lists..."
sudo apt-get update

# Install dependencies
echo "Installing build tools and PostgreSQL..."
sudo apt-get install -y \
    build-essential \
    cmake \
    postgresql \
    postgresql-client \
    libpq-dev \
    libpqxx-dev \
    pkg-config

echo "✓ Dependencies installed"

# Check PostgreSQL
echo ""
echo "Checking PostgreSQL..."
if ! command -v psql &> /dev/null; then
    echo "✗ PostgreSQL not found after installation"
    exit 1
fi
echo "✓ PostgreSQL found"

# Start PostgreSQL
echo ""
echo "Starting PostgreSQL service..."
sudo service postgresql start
echo "✓ PostgreSQL service started"

# Create database
echo ""
echo "Setting up database..."
sudo -u postgres psql -c "CREATE DATABASE competence_analyzer;" 2>/dev/null || echo "Database might already exist"

# Load schema
echo "Loading database schema..."
sudo -u postgres psql competence_analyzer < sql/schema.sql
echo "✓ Schema loaded"

# Load test data
echo "Loading test data..."
sudo -u postgres psql competence_analyzer < sql/test_data.sql
echo "✓ Test data loaded"

echo ""
echo "=========================================="
echo "✓ Installation Complete"
echo "=========================================="
echo ""
echo "Database Details:"
echo "  Host: localhost"
echo "  Port: 5432"
echo "  Database: competence_analyzer"
echo "  User: postgres"
echo "  Password: [default PostgreSQL password]"
echo ""
echo "Next steps:"
echo "1. Run build script: ./build.sh"
echo "2. Execute: ./build/competence-analyzer"
echo ""
