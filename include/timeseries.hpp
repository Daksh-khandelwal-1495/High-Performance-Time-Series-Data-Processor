#pragma once

#include "record.hpp"
#include <vector>
#include <string>

namespace tsproc {

/**
 * @brief In-memory time-series container
 * 
 * Stores a sequence of Record objects representing time-series data.
 * Provides efficient access and utility methods for extracting columns.
 */
class TimeSeries {
public:
    /**
     * @brief Add a record to the end of the time series
     */
    void push(const Record& r);

    /**
     * @brief Get the number of records in the time series
     */
    size_t size() const;

    /**
     * @brief Check if time series is empty
     */
    bool empty() const;

    /**
     * @brief Access record by index (non-const)
     */
    Record& operator[](size_t i);

    /**
     * @brief Access record by index (const)
     */
    const Record& operator[](size_t i) const;

    /**
     * @brief Get begin iterator
     */
    std::vector<Record>::iterator begin();
    std::vector<Record>::const_iterator begin() const;

    /**
     * @brief Get end iterator
     */
    std::vector<Record>::iterator end();
    std::vector<Record>::const_iterator end() const;

    /**
     * @brief Extract close price series as vector
     * 
     * Convenience method for indicator calculations that need
     * a continuous series of values.
     */
    std::vector<double> get_close_series() const;

    /**
     * @brief Extract any OHLCV column as vector
     * 
     * @param col Column name: "open", "high", "low", "close", "adj_close", "volume"
     * @return Vector of values from the specified column
     */
    std::vector<double> get_column(const std::string& col) const;

    /**
     * @brief Reserve capacity for records
     * 
     * Useful when you know the approximate size in advance to avoid reallocations
     */
    void reserve(size_t capacity);

    /**
     * @brief Clear all records
     */
    void clear();

private:
    std::vector<Record> rows_;
};

} // namespace tsproc
