#!/bin/bash
# Manual build script without CMake
# For Windows with MinGW/GCC

set -e

echo "==========================================="
echo "Building Time-Series Processor (Manual)"
echo "==========================================="

# Compiler settings
CXX="g++"
CXXFLAGS="-std=c++17 -O3 -Wall -Wextra -Wpedantic -Iinclude"
LDFLAGS=""

# Create output directories
mkdir -p build/obj
mkdir -p build/bin

echo "Compiling source files..."

# Compile library sources
echo "  -> csv_reader.cpp"
$CXX $CXXFLAGS -c src/csv_reader.cpp -o build/obj/csv_reader.o

echo "  -> timeseries.cpp"
$CXX $CXXFLAGS -c src/timeseries.cpp -o build/obj/timeseries.o

echo "  -> indicators.cpp"
$CXX $CXXFLAGS -c src/indicators.cpp -o build/obj/indicators.o

echo "  -> signals.cpp"
$CXX $CXXFLAGS -c src/signals.cpp -o build/obj/signals.o

echo "  -> io.cpp"
$CXX $CXXFLAGS -c src/io.cpp -o build/obj/io.o

echo "  -> main.cpp"
$CXX $CXXFLAGS -c src/main.cpp -o build/obj/main.o

echo ""
echo "Linking executable..."
$CXX build/obj/*.o -o build/bin/tsproc.exe $LDFLAGS

echo ""
echo "==========================================="
echo "Build complete!"
echo "==========================================="
echo ""
echo "Executable: ./build/bin/tsproc.exe"
echo ""
echo "Run with:"
echo "  ./build/bin/tsproc.exe --input data/sample.csv --output output.csv --sma 5"
echo ""
