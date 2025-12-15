#include "csv_reader.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>

namespace tsproc {

CSVReader::CSVReader(const std::string& path, char delimiter)
    : path_(path), delimiter_(delimiter) {
}

bool CSVReader::is_open() const {
    std::ifstream file(path_);
    return file.is_open();
}

std::vector<std::string> CSVReader::split_line_fast(const std::string& line, char delim) const {
    std::vector<std::string> out;
    out.reserve(8);  // Typical OHLCV has 7 columns
    size_t start = 0;
    
    for (size_t i = 0; i <= line.size(); ++i) {
        if (i == line.size() || line[i] == delim) {
            out.emplace_back(line.substr(start, i - start));
            start = i + 1;
        }
    }
    
    return out;
}

std::string CSVReader::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

double CSVReader::safe_stod(const std::string& str) const {
    try {
        std::string trimmed = trim(str);
        if (trimmed.empty()) return NAN;
        return std::stod(trimmed);
    } catch (...) {
        return NAN;
    }
}

bool CSVReader::parse_record(const std::vector<std::string>& columns, Record& record) const {
    // Expect at least 7 columns: Date, Open, High, Low, Close, Adj Close, Volume
    if (columns.size() < 7) {
        return false;
    }

    record.date = trim(columns[0]);
    record.open = safe_stod(columns[1]);
    record.high = safe_stod(columns[2]);
    record.low = safe_stod(columns[3]);
    record.close = safe_stod(columns[4]);
    record.adj_close = safe_stod(columns[5]);
    record.volume = safe_stod(columns[6]);

    // Check for NaN values
    bool has_nan = std::isnan(record.open) || std::isnan(record.high) ||
                   std::isnan(record.low) || std::isnan(record.close) ||
                   std::isnan(record.adj_close) || std::isnan(record.volume);

    return !has_nan;
}

TimeSeries CSVReader::read_to_timeseries(bool drop_na) {
    TimeSeries ts;
    std::ifstream file(path_);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << path_ << std::endl;
        return ts;
    }

    std::string line;
    bool is_header = true;
    size_t line_num = 0;

    while (std::getline(file, line)) {
        line_num++;
        
        // Skip header row
        if (is_header) {
            is_header = false;
            continue;
        }

        // Skip empty lines
        if (trim(line).empty()) {
            continue;
        }

        std::vector<std::string> columns = split_line_fast(line, delimiter_);
        Record record;
        
        bool valid = parse_record(columns, record);
        
        if (!valid && drop_na) {
            // Skip invalid records
            continue;
        } else if (!valid && !drop_na) {
            // Keep invalid records with NaN values
            record.date = columns.size() > 0 ? trim(columns[0]) : "";
        }
        
        ts.push(record);
    }

    file.close();
    return ts;
}

void CSVReader::stream_to(std::function<void(const Record&)> callback, bool drop_na) {
    std::ifstream file(path_);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << path_ << std::endl;
        return;
    }

    std::string line;
    bool is_header = true;

    while (std::getline(file, line)) {
        // Skip header row
        if (is_header) {
            is_header = false;
            continue;
        }

        // Skip empty lines
        if (trim(line).empty()) {
            continue;
        }

        std::vector<std::string> columns = split_line_fast(line, delimiter_);
        Record record;
        
        bool valid = parse_record(columns, record);
        
        if (valid || !drop_na) {
            callback(record);
        }
    }

    file.close();
}

} // namespace tsproc
