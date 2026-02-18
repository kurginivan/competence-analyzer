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

echo "Building all services with docker-compose..."
docker-compose build

echo ""
echo "=========================================="
echo "✓ Build completed!"
echo "=========================================="
echo ""
echo "Next: Run with Docker Compose"
echo "  docker-compose up -d"
echo ""
