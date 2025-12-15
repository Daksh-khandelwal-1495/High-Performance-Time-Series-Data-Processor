#include "io.hpp"
#include <fstream>
#include <iostream>
#include <set>
#include <algorithm>
#include <cmath>

namespace tsproc {

// ============================================================================
// CSVWriter Implementation
// ============================================================================

CSVWriter::CSVWriter(const std::string& out_path) : out_path_(out_path) {}

std::string CSVWriter::escape_csv_field(const std::string& field) const {
    // If field contains comma, quote, or newline, escape it
    if (field.find(',') != std::string::npos ||
        field.find('"') != std::string::npos ||
        field.find('\n') != std::string::npos) {
        
        std::string escaped = "\"";
        for (char c : field) {
            if (c == '"') escaped += "\"\"";  // Escape quotes by doubling
            else escaped += c;
        }
        escaped += "\"";
        return escaped;
    }
    return field;
}

std::vector<std::string> CSVWriter::collect_indicator_names(const TimeSeries& ts) const {
    std::set<std::string> indicator_set;
    
    for (size_t i = 0; i < ts.size(); ++i) {
        for (const auto& pair : ts[i].indicators) {
            indicator_set.insert(pair.first);
        }
    }
    
    return std::vector<std::string>(indicator_set.begin(), indicator_set.end());
}

bool CSVWriter::write(const TimeSeries& ts, const std::vector<std::string>& extra_cols) {
    std::ofstream file(out_path_);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file: " << out_path_ << std::endl;
        return false;
    }

    // Collect all indicator names
    std::vector<std::string> indicators = collect_indicator_names(ts);
    
    // Merge with extra_cols (if specified, use only those)
    if (!extra_cols.empty()) {
        indicators = extra_cols;
    }

    // Write header
    file << "Date,Open,High,Low,Close,Adj Close,Volume,Signal";
    for (const auto& ind : indicators) {
        file << "," << ind;
    }
    file << "\n";

    // Write data rows
    for (size_t i = 0; i < ts.size(); ++i) {
        const Record& r = ts[i];
        
        file << escape_csv_field(r.date) << ","
             << r.open << ","
             << r.high << ","
             << r.low << ","
             << r.close << ","
             << r.adj_close << ","
             << r.volume << ","
             << r.signal;
        
        // Write indicator values
        for (const auto& ind_name : indicators) {
            file << ",";
            auto it = r.indicators.find(ind_name);
            if (it != r.indicators.end() && !std::isnan(it->second)) {
                file << it->second;
            } else {
                file << "NaN";
            }
        }
        
        file << "\n";
    }

    file.close();
    return true;
}

bool CSVWriter::write_columns(const TimeSeries& ts, const std::vector<std::string>& columns) {
    std::ofstream file(out_path_);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file: " << out_path_ << std::endl;
        return false;
    }

    // Write header
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i > 0) file << ",";
        file << columns[i];
    }
    file << "\n";

    // Write data rows (simplified - only OHLCV columns)
    for (size_t i = 0; i < ts.size(); ++i) {
        const Record& r = ts[i];
        
        for (size_t j = 0; j < columns.size(); ++j) {
            if (j > 0) file << ",";
            
            const std::string& col = columns[j];
            if (col == "Date") file << escape_csv_field(r.date);
            else if (col == "Open") file << r.open;
            else if (col == "High") file << r.high;
            else if (col == "Low") file << r.low;
            else if (col == "Close") file << r.close;
            else if (col == "Adj Close") file << r.adj_close;
            else if (col == "Volume") file << r.volume;
            else if (col == "Signal") file << r.signal;
            else {
                // Try to find in indicators
                auto it = r.indicators.find(col);
                if (it != r.indicators.end() && !std::isnan(it->second)) {
                    file << it->second;
                } else {
                    file << "NaN";
                }
            }
        }
        file << "\n";
    }

    file.close();
    return true;
}

// ============================================================================
// BinaryWriter Implementation
// ============================================================================

BinaryWriter::BinaryWriter(const std::string& out_path) : out_path_(out_path) {}

bool BinaryWriter::write(const TimeSeries& ts, bool include_indicators) {
    std::ofstream file(out_path_, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open binary output file: " << out_path_ << std::endl;
        return false;
    }

    // Write dimensions
    uint64_t num_rows = ts.size();
    uint64_t num_cols = 7;  // OHLCV + date (as index) + signal
    
    file.write(reinterpret_cast<const char*>(&num_rows), sizeof(num_rows));
    file.write(reinterpret_cast<const char*>(&num_cols), sizeof(num_cols));

    // Write data
    for (size_t i = 0; i < ts.size(); ++i) {
        const Record& r = ts[i];
        
        // Write OHLCV data as doubles
        file.write(reinterpret_cast<const char*>(&r.open), sizeof(double));
        file.write(reinterpret_cast<const char*>(&r.high), sizeof(double));
        file.write(reinterpret_cast<const char*>(&r.low), sizeof(double));
        file.write(reinterpret_cast<const char*>(&r.close), sizeof(double));
        file.write(reinterpret_cast<const char*>(&r.adj_close), sizeof(double));
        file.write(reinterpret_cast<const char*>(&r.volume), sizeof(double));
        
        // Write signal as double
        double signal_d = static_cast<double>(r.signal);
        file.write(reinterpret_cast<const char*>(&signal_d), sizeof(double));
    }

    file.close();
    return true;
}

// ============================================================================
// BinaryReader Implementation
// ============================================================================

BinaryReader::BinaryReader(const std::string& path) : path_(path) {}

TimeSeries BinaryReader::read() {
    TimeSeries ts;
    std::ifstream file(path_, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open binary input file: " << path_ << std::endl;
        return ts;
    }

    // Read dimensions
    uint64_t num_rows, num_cols;
    file.read(reinterpret_cast<char*>(&num_rows), sizeof(num_rows));
    file.read(reinterpret_cast<char*>(&num_cols), sizeof(num_cols));

    ts.reserve(num_rows);

    // Read data
    for (uint64_t i = 0; i < num_rows; ++i) {
        Record r;
        
        file.read(reinterpret_cast<char*>(&r.open), sizeof(double));
        file.read(reinterpret_cast<char*>(&r.high), sizeof(double));
        file.read(reinterpret_cast<char*>(&r.low), sizeof(double));
        file.read(reinterpret_cast<char*>(&r.close), sizeof(double));
        file.read(reinterpret_cast<char*>(&r.adj_close), sizeof(double));
        file.read(reinterpret_cast<char*>(&r.volume), sizeof(double));
        
        double signal_d;
        file.read(reinterpret_cast<char*>(&signal_d), sizeof(double));
        r.signal = static_cast<int>(signal_d);
        
        ts.push(r);
    }

    file.close();
    return ts;
}

} // namespace tsproc
