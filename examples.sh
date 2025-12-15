#!/bin/bash
# Example usage demonstrations for Time-Series Processor

echo "================================================"
echo "Time-Series Processor - Example Demonstrations"
echo "================================================"
echo ""

# Check if binary exists
if [ ! -f "./build/bin/tsproc" ]; then
    echo "Error: tsproc binary not found. Please build the project first:"
    echo "  ./build.sh"
    exit 1
fi

# Check if sample data exists
if [ ! -f "./data/sample.csv" ]; then
    echo "Error: sample.csv not found in data/ directory"
    exit 1
fi

TSPROC="./build/bin/tsproc"
INPUT="./data/sample.csv"
OUTPUT_DIR="./output"

# Create output directory
mkdir -p "$OUTPUT_DIR"

echo "Using sample data: $INPUT"
echo "Output directory: $OUTPUT_DIR"
echo ""

# Example 1: Basic SMA
echo "Example 1: Computing SMA (5 and 10 period)"
echo "-------------------------------------------"
$TSPROC --input "$INPUT" --output "$OUTPUT_DIR/example1_sma.csv" \
    --sma 5 --sma 10
echo "✓ Output: $OUTPUT_DIR/example1_sma.csv"
echo ""

# Example 2: Z-Score
echo "Example 2: Computing Z-Score (20 period)"
echo "-----------------------------------------"
$TSPROC --input "$INPUT" --output "$OUTPUT_DIR/example2_zscore.csv" \
    --zwindow 20
echo "✓ Output: $OUTPUT_DIR/example2_zscore.csv"
echo ""

# Example 3: SMA Crossover Signal
echo "Example 3: SMA Crossover Signal (fast=5, slow=10)"
echo "--------------------------------------------------"
$TSPROC --input "$INPUT" --output "$OUTPUT_DIR/example3_sma_crossover.csv" \
    --fast-sma 5 --slow-sma 10 --signal-sma
echo "✓ Output: $OUTPUT_DIR/example3_sma_crossover.csv"
echo ""

# Example 4: Z-Score Mean Reversion Signal
echo "Example 4: Z-Score Mean Reversion (entry=1.5, exit=0.5)"
echo "--------------------------------------------------------"
$TSPROC --input "$INPUT" --output "$OUTPUT_DIR/example4_zscore_signal.csv" \
    --zwindow 10 --signal-z --zentry 1.5 --zexit 0.5
echo "✓ Output: $OUTPUT_DIR/example4_zscore_signal.csv"
echo ""

# Example 5: Comprehensive Analysis
echo "Example 5: Comprehensive Analysis (all indicators + signals)"
echo "-------------------------------------------------------------"
$TSPROC --input "$INPUT" --output "$OUTPUT_DIR/example5_comprehensive.csv" \
    --sma 5 --sma 10 --sma 20 \
    --zwindow 10 \
    --fast-sma 5 --slow-sma 10 --signal-sma \
    --binary
echo "✓ Output: $OUTPUT_DIR/example5_comprehensive.csv"
echo "✓ Binary: $OUTPUT_DIR/example5_comprehensive.csv.bin"
echo ""

echo "================================================"
echo "All examples completed successfully!"
echo "================================================"
echo ""
echo "View results:"
echo "  ls -lh $OUTPUT_DIR/"
echo ""
echo "Inspect CSV output:"
echo "  head $OUTPUT_DIR/example1_sma.csv"
echo "  cat $OUTPUT_DIR/example3_sma_crossover.csv"
echo ""
