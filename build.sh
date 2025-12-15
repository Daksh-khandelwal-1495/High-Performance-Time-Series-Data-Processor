#!/bin/bash
# Build script for High-Performance Time-Series Data Processor

set -e

echo "==================================="
echo "Building Time-Series Processor"
echo "==================================="

# Create build directory
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building project..."
cmake --build . --config Release -j4

echo ""
echo "==================================="
echo "Build complete!"
echo "==================================="
echo ""
echo "Executables:"
echo "  - ./bin/tsproc       (Main CLI tool)"
echo "  - ./bin/runTests     (Unit tests)"
echo ""
echo "Run tests with: ctest --output-on-failure"
echo "Or directly: ./bin/runTests"
echo ""
