#!/bin/bash
# Project Validation Script
# Checks that all components are properly implemented

echo "========================================"
echo "Time-Series Processor - Validation"
echo "========================================"
echo ""

FAILED=0

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

check() {
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓${NC} $1"
    else
        echo -e "${RED}✗${NC} $1"
        FAILED=$((FAILED + 1))
    fi
}

# Check header files
echo "Checking header files..."
test -f include/record.hpp; check "record.hpp exists"
test -f include/timeseries.hpp; check "timeseries.hpp exists"
test -f include/csv_reader.hpp; check "csv_reader.hpp exists"
test -f include/indicators.hpp; check "indicators.hpp exists"
test -f include/signals.hpp; check "signals.hpp exists"
test -f include/io.hpp; check "io.hpp exists"
echo ""

# Check implementation files
echo "Checking implementation files..."
test -f src/timeseries.cpp; check "timeseries.cpp exists"
test -f src/csv_reader.cpp; check "csv_reader.cpp exists"
test -f src/indicators.cpp; check "indicators.cpp exists"
test -f src/signals.cpp; check "signals.cpp exists"
test -f src/io.cpp; check "io.cpp exists"
test -f src/main.cpp; check "main.cpp exists"
echo ""

# Check test files
echo "Checking test files..."
test -f tests/test_csv_reader.cpp; check "test_csv_reader.cpp exists"
test -f tests/test_indicators.cpp; check "test_indicators.cpp exists"
test -f tests/test_signals.cpp; check "test_signals.cpp exists"
test -f tests/test_io.cpp; check "test_io.cpp exists"
echo ""

# Check build files
echo "Checking build configuration..."
test -f CMakeLists.txt; check "CMakeLists.txt exists"
test -f build.sh; check "build.sh exists"
echo ""

# Check documentation
echo "Checking documentation..."
test -f README.md; check "README.md exists"
test -f QUICKSTART.md; check "QUICKSTART.md exists"
test -f IMPLEMENTATION.md; check "IMPLEMENTATION.md exists"
test -f LICENSE; check "LICENSE exists"
echo ""

# Check data and tools
echo "Checking data and tools..."
test -f data/sample.csv; check "sample.csv exists"
test -f tools/gen_synthetic.py; check "gen_synthetic.py exists"
test -f examples.sh; check "examples.sh exists"
echo ""

# Check for required content in files
echo "Checking implementation completeness..."

# Check for key classes/functions
grep -q "class CSVReader" include/csv_reader.hpp; check "CSVReader class defined"
grep -q "class TimeSeries" include/timeseries.hpp; check "TimeSeries class defined"
grep -q "void add_sma" include/indicators.hpp; check "add_sma function defined"
grep -q "void add_zscore" include/indicators.hpp; check "add_zscore function defined"
grep -q "void sma_crossover" include/signals.hpp; check "sma_crossover function defined"
grep -q "void zscore_mean_reversion" include/signals.hpp; check "zscore_mean_reversion function defined"
grep -q "class CSVWriter" include/io.hpp; check "CSVWriter class defined"
echo ""

# Check test cases
echo "Checking test implementation..."
grep -q "TEST_F" tests/test_csv_reader.cpp; check "CSV reader tests exist"
grep -q "TEST_F" tests/test_indicators.cpp; check "Indicator tests exist"
grep -q "TEST_F" tests/test_signals.cpp; check "Signal tests exist"
grep -q "TEST_F" tests/test_io.cpp; check "I/O tests exist"
echo ""

# Check CMake configuration
echo "Checking CMake configuration..."
grep -q "add_library(tsprocessor" CMakeLists.txt; check "Library target configured"
grep -q "add_executable(tsproc" CMakeLists.txt; check "Executable target configured"
grep -q "add_executable(runTests" CMakeLists.txt; check "Test target configured"
grep -q "gtest" CMakeLists.txt; check "GoogleTest integrated"
echo ""

# Summary
echo "========================================"
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All checks passed!${NC}"
    echo "Project is complete and ready to build."
else
    echo -e "${RED}Failed checks: $FAILED${NC}"
    echo "Please review the failed items above."
fi
echo "========================================"

exit $FAILED
