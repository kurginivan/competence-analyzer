#!/bin/bash

# ============================================================
# Troubleshooting Script for Competence Analyzer
# ============================================================

set -e

echo "=========================================="
echo "Competence Analyzer - Troubleshooting"
echo "=========================================="
echo ""

# Check CMake
echo "1. Checking CMake..."
if command -v cmake &> /dev/null; then
    cmake --version
else
    echo "✗ CMake not found"
    exit 1
fi

# Check compiler
echo ""
echo "2. Checking C++ Compiler..."
if command -v g++ &> /dev/null; then
    g++ --version | head -1
else
    echo "✗ G++ not found"
    exit 1
fi

# Check pkg-config
echo ""
echo "3. Checking pkg-config..."
if command -v pkg-config &> /dev/null; then
    pkg-config --version
else
    echo "✗ pkg-config not found"
    exit 1
fi

# Check PostgreSQL
echo ""
echo "4. Checking PostgreSQL libraries..."
pkg-config --cflags --libs libpq
echo "✓ libpq found"

# Check libpqxx
echo ""
echo "5. Checking libpqxx..."
if pkg-config --exists libpqxx; then
    pkg-config --cflags --libs libpqxx
    echo "✓ libpqxx found"
else
    echo "✗ libpqxx not found"
    echo "Install with: sudo apt-get install libpqxx-dev"
    exit 1
fi

# Check if PostgreSQL server is running (optional)
echo ""
echo "6. Checking PostgreSQL server..."
if psql -U postgres -c "SELECT version();" > /dev/null 2>&1; then
    echo "✓ PostgreSQL server is running"
else
    echo "⚠ PostgreSQL server might not be running"
    echo "  Start with: sudo service postgresql start"
fi

echo ""
echo "=========================================="
echo "✓ All checks passed!"
echo "=========================================="
echo ""
echo "Try building with:"
echo "  mkdir -p build && cd build"
echo "  cmake -DCMAKE_VERBOSE_MAKEFILE=ON .."
echo "  make VERBOSE=1"
echo ""
