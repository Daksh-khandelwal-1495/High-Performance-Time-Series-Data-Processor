#include "indicators.hpp"
#include <deque>
#include <cmath>
#include <stdexcept>

namespace tsproc {
namespace indicators {

double get_column_value(const Record& r, const std::string& col) {
    if (col == "open") return r.open;
    if (col == "high") return r.high;
    if (col == "low") return r.low;
    if (col == "close") return r.close;
    if (col == "adj_close") return r.adj_close;
    if (col == "volume") return r.volume;
    throw std::invalid_argument("Unknown column: " + col);
}

void add_sma(TimeSeries& ts, size_t window, const std::string& col) {
    if (ts.size() == 0 || window == 0) return;
    
    std::deque<double> q;
    double sum = 0.0;
    std::string indicator_name = "SMA_" + std::to_string(window);
    
    for (size_t i = 0; i < ts.size(); ++i) {
        double val = get_column_value(ts[i], col);
        
        q.push_back(val);
        sum += val;
        
        if (q.size() > window) {
            sum -= q.front();
            q.pop_front();
        }
        
        if (q.size() == window) {
            double sma = sum / static_cast<double>(window);
            ts[i].indicators[indicator_name] = sma;
        } else {
            ts[i].indicators[indicator_name] = NAN;
        }
    }
}

void add_roll_mean_std(TimeSeries& ts, size_t window, const std::string& col) {
    if (ts.size() == 0 || window == 0) return;
    
    std::deque<double> q;
    double sum = 0.0;
    double sumsq = 0.0;
    std::string mean_name = "ROLL_MEAN_" + std::to_string(window);
    std::string std_name = "ROLL_STD_" + std::to_string(window);
    
    for (size_t i = 0; i < ts.size(); ++i) {
        double v = get_column_value(ts[i], col);
        
        q.push_back(v);
        sum += v;
        sumsq += v * v;
        
        if (q.size() > window) {
            double old = q.front();
            q.pop_front();
            sum -= old;
            sumsq -= old * old;
        }
        
        if (q.size() == window) {
            double mean = sum / static_cast<double>(window);
            double variance = (sumsq / static_cast<double>(window)) - (mean * mean);
            double sd = (variance > 0) ? std::sqrt(variance) : 0.0;
            
            ts[i].indicators[mean_name] = mean;
            ts[i].indicators[std_name] = sd;
        } else {
            ts[i].indicators[mean_name] = NAN;
            ts[i].indicators[std_name] = NAN;
        }
    }
}

void add_zscore(TimeSeries& ts, size_t window, const std::string& col) {
    if (ts.size() == 0 || window == 0) return;
    
    // First compute rolling mean and std if not already present
    std::string mean_name = "ROLL_MEAN_" + std::to_string(window);
    std::string std_name = "ROLL_STD_" + std::to_string(window);
    
    // Check if we need to compute mean and std
    if (ts.size() > 0 && ts[0].indicators.find(mean_name) == ts[0].indicators.end()) {
        add_roll_mean_std(ts, window, col);
    }
    
    std::string zscore_name = "Z_" + std::to_string(window);
    
    for (size_t i = 0; i < ts.size(); ++i) {
        auto& indicators = ts[i].indicators;
        
        if (indicators.find(mean_name) != indicators.end() && 
            indicators.find(std_name) != indicators.end()) {
            
            double mean = indicators[mean_name];
            double sd = indicators[std_name];
            double val = get_column_value(ts[i], col);
            
            if (!std::isnan(mean) && !std::isnan(sd) && sd > 1e-10) {
                double zscore = (val - mean) / sd;
                indicators[zscore_name] = zscore;
            } else {
                indicators[zscore_name] = NAN;
            }
        } else {
            indicators[zscore_name] = NAN;
        }
    }
}

void add_ema(TimeSeries& ts, size_t window, const std::string& col) {
    if (ts.size() == 0 || window == 0) return;
    
    double alpha = 2.0 / (static_cast<double>(window) + 1.0);
    std::string indicator_name = "EMA_" + std::to_string(window);
    
    double ema = 0.0;
    bool initialized = false;
    
    for (size_t i = 0; i < ts.size(); ++i) {
        double val = get_column_value(ts[i], col);
        
        if (!initialized) {
            ema = val;
            initialized = true;
        } else {
            ema = alpha * val + (1.0 - alpha) * ema;
        }
        
        ts[i].indicators[indicator_name] = ema;
    }
}

void add_roll_sum(TimeSeries& ts, size_t window, const std::string& col) {
    if (ts.size() == 0 || window == 0) return;
    
    std::deque<double> q;
    double sum = 0.0;
    std::string indicator_name = "ROLL_SUM_" + std::to_string(window);
    
    for (size_t i = 0; i < ts.size(); ++i) {
        double val = get_column_value(ts[i], col);
        
        q.push_back(val);
        sum += val;
        
        if (q.size() > window) {
            sum -= q.front();
            q.pop_front();
        }
        
        if (q.size() == window) {
            ts[i].indicators[indicator_name] = sum;
        } else {
            ts[i].indicators[indicator_name] = NAN;
        }
    }
}

void add_volatility(TimeSeries& ts, size_t window, const std::string& col, 
                    double periods_per_year) {
    if (ts.size() == 0 || window == 0) return;
    
    // First compute rolling std if not present
    std::string std_name = "ROLL_STD_" + std::to_string(window);
    
    if (ts.size() > 0 && ts[0].indicators.find(std_name) == ts[0].indicators.end()) {
        add_roll_mean_std(ts, window, col);
    }
    
    std::string vol_name = "VOL_" + std::to_string(window);
    double annualization_factor = std::sqrt(periods_per_year);
    
    for (size_t i = 0; i < ts.size(); ++i) {
        auto& indicators = ts[i].indicators;
        
        if (indicators.find(std_name) != indicators.end()) {
            double sd = indicators[std_name];
            if (!std::isnan(sd)) {
                indicators[vol_name] = sd * annualization_factor;
            } else {
                indicators[vol_name] = NAN;
            }
        } else {
            indicators[vol_name] = NAN;
        }
    }
}

} // namespace indicators
} // namespace tsproc
