#pragma once

#include <string>
#include <unordered_map>

namespace tsproc {

/**
 * @brief Basic row record for time-series data
 * 
 * Represents a single time-series data point with OHLCV data
 * and computed indicators stored in a hash map for flexibility.
 */
struct Record {
    std::string date;   ///< Date string (ISO format: YYYY-MM-DD or YYYY-MM-DD HH:MM:SS)
    double open;        ///< Opening price
    double high;        ///< Highest price
    double low;         ///< Lowest price
    double close;       ///< Closing price
    double adj_close;   ///< Adjusted closing price
    double volume;      ///< Trading volume

    /// Computed fields (e.g., "SMA_20" -> value, "Z_20" -> z-score)
    std::unordered_map<std::string, double> indicators;
    
    /// Signal value: -1 (short), 0 (flat/no position), +1 (long)
    int signal = 0;

    /**
     * @brief Default constructor initializes numeric fields to 0
     */
    Record() : open(0.0), high(0.0), low(0.0), close(0.0), 
               adj_close(0.0), volume(0.0), signal(0) {}
};

} // namespace tsproc
