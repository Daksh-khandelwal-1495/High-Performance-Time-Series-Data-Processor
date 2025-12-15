# High-Performance Time-Series Data Processor
## Quick Start Guide

### Project Structure
```
cpp-timeseries-processor/
├── include/           # Header files (API)
├── src/              # Implementation files
├── tests/            # Unit tests (GoogleTest)
├── data/             # Sample CSV data
├── tools/            # Helper scripts
├── CMakeLists.txt    # Build configuration
└── README.md         # Full documentation
```

### Build Instructions

**Prerequisites:**
- CMake 3.14+
- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)

**Steps:**
```bash
# Using build script (Linux/Mac)
chmod +x build.sh
./build.sh

# Or manually
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j4
```

**Windows (Visual Studio):**
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

### Quick Test

```bash
# Run with sample data
./build/bin/tsproc --input data/sample.csv --output output.csv --sma 5 --sma 10

# Run unit tests
cd build
ctest --output-on-failure
```

### Core Components Implemented

✅ **CSVReader** (`csv_reader.hpp/cpp`)
   - Fast O(n) parsing
   - Handles missing values
   - Case-insensitive headers
   - Custom delimiters

✅ **TimeSeries** (`timeseries.hpp/cpp`)
   - In-memory container
   - Vector-based storage
   - Column extraction utilities

✅ **Indicators** (`indicators.hpp/cpp`)
   - Simple Moving Average (SMA)
   - Rolling Mean & Std Dev
   - Z-Score calculation
   - O(n) time, O(k) space complexity

✅ **Signals** (`signals.hpp/cpp`)
   - SMA Crossover (Golden/Death Cross)
   - Z-Score Mean Reversion
   - Threshold-based signals

✅ **I/O** (`io.hpp/cpp`)
   - CSV writer with indicator columns
   - Binary format for fast serialization
   - NaN handling

✅ **CLI** (`main.cpp`)
   - Argument parsing
   - Multiple indicator support
   - Signal generation
   - Binary output option

✅ **Tests** (`tests/`)
   - CSV parsing edge cases
   - Indicator correctness (test vectors)
   - Signal generation logic
   - I/O format consistency

### Usage Examples

**1. Basic SMA Calculation**
```bash
./bin/tsproc --input data.csv --output out.csv --sma 20 --sma 50
```

**2. Z-Score Mean Reversion Strategy**
```bash
./bin/tsproc --input data.csv --output out.csv \
  --zwindow 20 --signal-z --zentry 2.0 --zexit 0.5
```

**3. SMA Crossover Strategy**
```bash
./bin/tsproc --input data.csv --output out.csv \
  --fast-sma 10 --slow-sma 50 --signal-sma
```

**4. Combined Analysis**
```bash
./bin/tsproc --input data.csv --output out.csv \
  --sma 20 --sma 50 --zwindow 20 \
  --fast-sma 10 --slow-sma 50 --signal-sma \
  --binary
```

### Generate Test Data

```bash
cd tools
python gen_synthetic.py -n 100000 -o ../data/large_test.csv
```

### Performance Characteristics

| Dataset Size | Parse Time | SMA(20) | Z-Score(20) | Total |
|--------------|------------|---------|-------------|-------|
| 10K rows     | ~5ms       | ~1ms    | ~2ms        | ~10ms |
| 100K rows    | ~50ms      | ~10ms   | ~20ms       | ~100ms|
| 1M rows      | ~500ms     | ~100ms  | ~200ms      | ~1s   |

*Approximate times on typical laptop (i7, 16GB RAM)*

### Algorithm Complexity

- **CSV Parsing**: O(n) time, O(1) extra memory
- **SMA**: O(n) time, O(k) space (k = window size)
- **Rolling Stats**: O(n) time, O(k) space
- **Z-Score**: O(n) time
- **Signals**: O(n) time

### Test Coverage

- ✅ CSV parsing (basic, edge cases, missing values)
- ✅ Indicators (SMA, rolling mean/std, z-score)
- ✅ Signals (crossover, mean reversion, thresholds)
- ✅ I/O (CSV write, binary write, NaN handling)

**Run tests:**
```bash
cd build
./bin/runTests
# Or with CTest
ctest --output-on-failure --verbose
```

### API Example (Programmatic Use)

```cpp
#include "csv_reader.hpp"
#include "indicators.hpp"
#include "signals.hpp"
#include "io.hpp"

int main() {
    // Load data
    tsproc::CSVReader reader("data.csv");
    tsproc::TimeSeries ts = reader.read_to_timeseries(true);
    
    // Compute indicators
    tsproc::indicators::add_sma(ts, 20, "close");
    tsproc::indicators::add_sma(ts, 50, "close");
    tsproc::indicators::add_zscore(ts, 20, "close");
    
    // Generate signals
    tsproc::signals::sma_crossover(ts, 20, 50, "signal_sma");
    tsproc::signals::zscore_mean_reversion(ts, 20, 2.0, 0.5, "signal_z");
    
    // Write output
    tsproc::CSVWriter writer("output.csv");
    writer.write(ts);
    
    return 0;
}
```

### Next Steps

1. **Build the project** using instructions above
2. **Run tests** to verify implementation
3. **Try with sample data** in `data/sample.csv`
4. **Generate larger datasets** using `tools/gen_synthetic.py`
5. **Integrate into your workflow** using the API

### Troubleshooting

**CMake not found:**
- Install CMake: https://cmake.org/download/
- Or use package manager: `apt install cmake` / `brew install cmake`

**Compiler errors:**
- Ensure C++17 support (GCC 7+, Clang 5+, MSVC 2017+)
- Update compiler: `sudo apt install g++` / `xcode-select --install`

**Test failures:**
- Check test output: `./bin/runTests --gtest_verbose`
- Verify CSV format matches expected structure

### License

MIT License - See LICENSE file

### Author

Daksh Khandelwal - 2025
