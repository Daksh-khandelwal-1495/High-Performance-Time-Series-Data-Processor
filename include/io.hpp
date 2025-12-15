#pragma once

#include "timeseries.hpp"
#include <string>
#include <vector>

namespace tsproc {

/**
 * @brief CSV writer for exporting processed time-series data
 * 
 * Writes TimeSeries data to CSV format with OHLCV columns plus
 * any computed indicators and signals.
 */
class CSVWriter {
public:
    /**
     * @brief Construct a CSV writer
     * 
     * @param out_path Output file path
     */
    explicit CSVWriter(const std::string& out_path);

    /**
     * @brief Write TimeSeries to CSV file
     * 
     * Automatically includes OHLCV columns and any indicators present.
     * 
     * @param ts TimeSeries to write
     * @param extra_cols Additional indicator column names to include (optional)
     * @return true if write succeeded, false otherwise
     */
    bool write(const TimeSeries& ts, const std::vector<std::string>& extra_cols = {});

    /**
     * @brief Write TimeSeries with specific columns only
     * 
     * @param ts TimeSeries to write
     * @param columns Specific columns to include
     * @return true if write succeeded, false otherwise
     */
    bool write_columns(const TimeSeries& ts, const std::vector<std::string>& columns);

private:
    std::string out_path_;

    /**
     * @brief Collect all unique indicator names from the time series
     */
    std::vector<std::string> collect_indicator_names(const TimeSeries& ts) const;

    /**
     * @brief Escape CSV field if needed (contains comma, quote, or newline)
     */
    std::string escape_csv_field(const std::string& field) const;
};

/**
 * @brief Binary writer for compact storage
 * 
 * Writes time-series data in a simple binary format for faster I/O.
 * Format: [header: num_rows, num_cols] [col_names] [data_matrix]
 */
class BinaryWriter {
public:
    /**
     * @brief Construct a binary writer
     * 
     * @param out_path Output file path
     */
    explicit BinaryWriter(const std::string& out_path);

    /**
     * @brief Write TimeSeries to binary file
     * 
     * @param ts TimeSeries to write
     * @param include_indicators If true, write indicator columns
     * @return true if write succeeded, false otherwise
     */
    bool write(const TimeSeries& ts, bool include_indicators = true);

private:
    std::string out_path_;
};

/**
 * @brief Binary reader for loading binary time-series files
 */
class BinaryReader {
public:
    /**
     * @brief Construct a binary reader
     * 
     * @param path Input file path
     */
    explicit BinaryReader(const std::string& path);

    /**
     * @brief Read binary file into TimeSeries
     * 
     * @return TimeSeries object with loaded data
     */
    TimeSeries read();

private:
    std::string path_;
};

} // namespace tsproc
