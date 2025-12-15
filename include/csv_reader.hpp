#pragma once

#include "timeseries.hpp"
#include <string>
#include <functional>

namespace tsproc {

/**
 * @brief Fast CSV reader for time-series OHLCV data
 * 
 * Efficiently parses CSV files with OHLCV data format.
 * Supports both batch loading and streaming modes.
 * 
 * Expected CSV format:
 * Date,Open,High,Low,Close,Adj Close,Volume
 * 2020-01-01,123.45,125.00,122.50,124.00,124.00,1000000
 */
class CSVReader {
public:
    /**
     * @brief Construct a CSV reader
     * 
     * @param path Path to CSV file
     * @param delimiter Column delimiter (default: comma)
     */
    explicit CSVReader(const std::string& path, char delimiter = ',');

    /**
     * @brief Parse entire CSV file into a TimeSeries object
     * 
     * @param drop_na If true, skip rows with missing/invalid numeric values
     * @return TimeSeries object containing all valid records
     */
    TimeSeries read_to_timeseries(bool drop_na = true);

    /**
     * @brief Stream CSV records using a callback function
     * 
     * More memory-efficient for very large files. Processes records
     * one at a time without storing all in memory.
     * 
     * @param callback Function called for each valid record
     * @param drop_na If true, skip rows with missing/invalid numeric values
     */
    void stream_to(std::function<void(const Record&)> callback, bool drop_na = true);

    /**
     * @brief Check if the file was opened successfully
     */
    bool is_open() const;

private:
    std::string path_;
    char delimiter_;

    /**
     * @brief Fast line splitting without stringstream overhead
     * 
     * @param line Input line to split
     * @param delim Delimiter character
     * @return Vector of column strings
     */
    std::vector<std::string> split_line_fast(const std::string& line, char delim) const;

    /**
     * @brief Parse a CSV row into a Record object
     * 
     * @param columns Vector of column strings
     * @param record Output record
     * @return true if parsing succeeded, false if invalid data
     */
    bool parse_record(const std::vector<std::string>& columns, Record& record) const;

    /**
     * @brief Trim whitespace from string
     */
    std::string trim(const std::string& str) const;

    /**
     * @brief Convert string to double, returns NaN on failure
     */
    double safe_stod(const std::string& str) const;
};

} // namespace tsproc
