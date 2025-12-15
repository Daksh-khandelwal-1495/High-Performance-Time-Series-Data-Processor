#include "timeseries.hpp"
#include <stdexcept>
#include <algorithm>

namespace tsproc {

void TimeSeries::push(const Record& r) {
    rows_.push_back(r);
}

size_t TimeSeries::size() const {
    return rows_.size();
}

bool TimeSeries::empty() const {
    return rows_.empty();
}

Record& TimeSeries::operator[](size_t i) {
    if (i >= rows_.size()) {
        throw std::out_of_range("TimeSeries index out of range");
    }
    return rows_[i];
}

const Record& TimeSeries::operator[](size_t i) const {
    if (i >= rows_.size()) {
        throw std::out_of_range("TimeSeries index out of range");
    }
    return rows_[i];
}

std::vector<Record>::iterator TimeSeries::begin() {
    return rows_.begin();
}

std::vector<Record>::const_iterator TimeSeries::begin() const {
    return rows_.begin();
}

std::vector<Record>::iterator TimeSeries::end() {
    return rows_.end();
}

std::vector<Record>::const_iterator TimeSeries::end() const {
    return rows_.end();
}

std::vector<double> TimeSeries::get_close_series() const {
    std::vector<double> result;
    result.reserve(rows_.size());
    for (const auto& row : rows_) {
        result.push_back(row.close);
    }
    return result;
}

std::vector<double> TimeSeries::get_column(const std::string& col) const {
    std::vector<double> result;
    result.reserve(rows_.size());
    
    for (const auto& row : rows_) {
        if (col == "open") {
            result.push_back(row.open);
        } else if (col == "high") {
            result.push_back(row.high);
        } else if (col == "low") {
            result.push_back(row.low);
        } else if (col == "close") {
            result.push_back(row.close);
        } else if (col == "adj_close") {
            result.push_back(row.adj_close);
        } else if (col == "volume") {
            result.push_back(row.volume);
        } else {
            throw std::invalid_argument("Unknown column: " + col);
        }
    }
    
    return result;
}

void TimeSeries::reserve(size_t capacity) {
    rows_.reserve(capacity);
}

void TimeSeries::clear() {
    rows_.clear();
}

} // namespace tsproc
