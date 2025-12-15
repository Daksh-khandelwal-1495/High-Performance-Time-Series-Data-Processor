#pragma once

#include "timeseries.hpp"
#include <string>

namespace tsproc {
namespace signals {

/**
 * @brief Simple Moving Average (SMA) crossover strategy
 * 
 * Generates trading signals based on fast/slow SMA crossover:
 * - Signal = +1 (long) when fast SMA crosses above slow SMA
 * - Signal = -1 (short) when fast SMA crosses below slow SMA
 * - Signal = 0 (no position) otherwise
 * 
 * Requires SMAs to be computed beforehand using indicators::add_sma()
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param fast_window Fast SMA window size
 * @param slow_window Slow SMA window size (must be > fast_window)
 * @param out_col Name for output signal indicator
 */
void sma_crossover(TimeSeries& ts, size_t fast_window, size_t slow_window,
                   const std::string& out_col = "signal_sma");

/**
 * @brief Z-score mean reversion strategy
 * 
 * Generates signals based on z-score thresholds:
 * - Enter long (+1) when z-score < -entry_z (oversold)
 * - Enter short (-1) when z-score > +entry_z (overbought)
 * - Exit (0) when |z-score| < exit_z (return to mean)
 * 
 * Typical values: entry_z = 2.0, exit_z = 0.5
 * Requires z-score to be computed using indicators::add_zscore()
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param window Z-score window size
 * @param entry_z Z-score threshold for entry (absolute value)
 * @param exit_z Z-score threshold for exit (absolute value)
 * @param out_col Name for output signal indicator
 */
void zscore_mean_reversion(TimeSeries& ts, size_t window, double entry_z, double exit_z,
                          const std::string& out_col = "signal_z");

/**
 * @brief Momentum strategy based on rate of change
 * 
 * Generates signals based on price momentum:
 * - Signal = +1 when momentum > upper_threshold
 * - Signal = -1 when momentum < lower_threshold
 * - Signal = 0 otherwise
 * 
 * Momentum = (price[t] - price[t-window]) / price[t-window]
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param window Lookback period for momentum calculation
 * @param upper_threshold Upper threshold for long signal (e.g., 0.05 for 5%)
 * @param lower_threshold Lower threshold for short signal (e.g., -0.05 for -5%)
 * @param col Column to compute momentum on (default: "close")
 * @param out_col Name for output signal indicator
 */
void momentum_strategy(TimeSeries& ts, size_t window, double upper_threshold, 
                      double lower_threshold, const std::string& col = "close",
                      const std::string& out_col = "signal_momentum");

/**
 * @brief Bollinger Band breakout strategy
 * 
 * Requires rolling mean and std to be computed beforehand.
 * Signals when price breaks outside num_std standard deviations from mean.
 * 
 * @param ts TimeSeries to process (modified in-place)
 * @param window Window size for mean and std
 * @param num_std Number of standard deviations for band (e.g., 2.0)
 * @param col Column to use (default: "close")
 * @param out_col Name for output signal indicator
 */
void bollinger_breakout(TimeSeries& ts, size_t window, double num_std,
                       const std::string& col = "close",
                       const std::string& out_col = "signal_bb");

} // namespace signals
} // namespace tsproc
