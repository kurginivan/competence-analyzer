#!/bin/bash

# ============================================================
# Docker Build Helper Script
# ============================================================

set -e

echo "=========================================="
echo "Competence Analyzer - Docker Build"
echo "=========================================="
echo ""

# Check Docker
if ! command -v docker &> /dev/null; then
    echo "✗ Docker not found. Please install Docker."
    exit 1
fi

if ! command -v docker-compose &> /dev/null; then
    echo "✗ Docker Compose not found. Please install Docker Compose."
    exit 1
fi

echo "✓ Docker and Docker Compose found"
echo ""

# Option to use multi-stage build
echo "Which build type would you prefer?"
echo "1. Standard Dockerfile (larger image, easier debugging)"
echo "2. Multi-stage Dockerfile (optimized, smaller image)"
echo ""
read -p "Enter choice (1 or 2) [default: 1]: " choice
choice=${choice:-1}

if [ "$choice" == "2" ]; then
    echo ""
    echo "Building with multi-stage Dockerfile..."
    docker build -f docker/Dockerfile.multistage -t competence-analyzer:latest .
else
    echo ""
    echo "Building with standard Dockerfile..."
    docker build -f docker/Dockerfile -t competence-analyzer:latest .
fi

echo ""
echo "=========================================="
echo "✓ Build completed!"
echo "=========================================="
echo ""
echo "Next: Run with Docker Compose"
echo "  docker-compose up -d"
echo ""
