#pragma once

#include "timeseries.hpp"
#include <string>

namespace tsproc {
namespace indicators {

/**
 * @brief Compute Simple Moving Average (SMA)
 * 
 * Adds "SMA_{window}" to each record's indicators map.
 * Uses O(n) time complexity with rolling window approach.
 * Records before window size have NaN values.
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param window Window size for SMA calculation
 * @param col Column name to compute SMA on (default: "close")
 */
void add_sma(TimeSeries& ts, size_t window, const std::string& col = "close");

/**
 * @brief Compute rolling mean and standard deviation
 * 
 * Adds "ROLL_MEAN_{window}" and "ROLL_STD_{window}" to indicators.
 * Uses Welford's algorithm variant for numerically stable computation.
 * O(n) time complexity, O(window) space complexity.
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param window Window size
 * @param col Column name to compute on (default: "close")
 */
void add_roll_mean_std(TimeSeries& ts, size_t window, const std::string& col = "close");

/**
 * @brief Compute rolling z-score
 * 
 * Z-score = (value - rolling_mean) / rolling_std
 * Adds "Z_{window}" to indicators map.
 * Requires rolling mean and std to be computed first.
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param window Window size
 * @param col Column name to compute on (default: "close")
 */
void add_zscore(TimeSeries& ts, size_t window, const std::string& col = "close");

/**
 * @brief Compute Exponential Moving Average (EMA)
 * 
 * EMA = α * value + (1-α) * previous_EMA
 * where α = 2 / (window + 1)
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param window Window size (affects smoothing factor)
 * @param col Column name to compute on (default: "close")
 */
void add_ema(TimeSeries& ts, size_t window, const std::string& col = "close");

/**
 * @brief Compute rolling sum
 * 
 * Adds "ROLL_SUM_{window}" to indicators.
 * Useful for volume analysis or other aggregations.
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param window Window size
 * @param col Column name to compute on (default: "volume")
 */
void add_roll_sum(TimeSeries& ts, size_t window, const std::string& col = "volume");

/**
 * @brief Compute rolling volatility (annualized)
 * 
 * Returns annualized volatility based on rolling standard deviation.
 * Assumes daily data, multiplies by sqrt(252) for annualization.
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param window Window size
 * @param col Column name to compute on (default: "close")
 * @param periods_per_year Trading periods per year (default: 252 for daily)
 */
void add_volatility(TimeSeries& ts, size_t window, const std::string& col = "close", 
                    double periods_per_year = 252.0);

/**
 * @brief Helper: Get column value from a record
 * 
 * Internal utility to extract numeric value from different columns
 */
double get_column_value(const Record& r, const std::string& col);

} // namespace indicators
} // namespace tsproc
