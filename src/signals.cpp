#include "signals.hpp"
#include "indicators.hpp"
#include <cmath>
#include <algorithm>

namespace tsproc {
namespace signals {

void sma_crossover(TimeSeries& ts, size_t fast_window, size_t slow_window,
                   const std::string& out_col) {
    if (ts.size() == 0 || fast_window >= slow_window) return;
    
    std::string fast_sma = "SMA_" + std::to_string(fast_window);
    std::string slow_sma = "SMA_" + std::to_string(slow_window);
    
    // Ensure SMAs are computed
    if (ts.size() > 0 && ts[0].indicators.find(fast_sma) == ts[0].indicators.end()) {
        indicators::add_sma(ts, fast_window, "close");
    }
    if (ts.size() > 0 && ts[0].indicators.find(slow_sma) == ts[0].indicators.end()) {
        indicators::add_sma(ts, slow_window, "close");
    }
    
    int prev_signal = 0;
    
    for (size_t i = 0; i < ts.size(); ++i) {
        auto& indicators = ts[i].indicators;
        
        if (indicators.find(fast_sma) != indicators.end() &&
            indicators.find(slow_sma) != indicators.end()) {
            
            double fast = indicators[fast_sma];
            double slow = indicators[slow_sma];
            
            if (!std::isnan(fast) && !std::isnan(slow)) {
                int signal = 0;
                
                // Check for crossover
                if (i > 0) {
                    auto& prev_ind = ts[i-1].indicators;
                    if (prev_ind.find(fast_sma) != prev_ind.end() &&
                        prev_ind.find(slow_sma) != prev_ind.end()) {
                        
                        double prev_fast = prev_ind[fast_sma];
                        double prev_slow = prev_ind[slow_sma];
                        
                        if (!std::isnan(prev_fast) && !std::isnan(prev_slow)) {
                            // Golden cross: fast crosses above slow
                            if (prev_fast <= prev_slow && fast > slow) {
                                signal = 1;
                            }
                            // Death cross: fast crosses below slow
                            else if (prev_fast >= prev_slow && fast < slow) {
                                signal = -1;
                            }
                            // Hold previous signal if no crossover
                            else {
                                signal = prev_signal;
                            }
                        }
                    }
                }
                
                indicators[out_col] = static_cast<double>(signal);
                ts[i].signal = signal;
                prev_signal = signal;
            } else {
                indicators[out_col] = 0.0;
                ts[i].signal = 0;
            }
        } else {
            indicators[out_col] = 0.0;
            ts[i].signal = 0;
        }
    }
}

void zscore_mean_reversion(TimeSeries& ts, size_t window, double entry_z, double exit_z,
                          const std::string& out_col) {
    if (ts.size() == 0) return;
    
    std::string zscore_name = "Z_" + std::to_string(window);
    
    // Ensure z-score is computed
    if (ts.size() > 0 && ts[0].indicators.find(zscore_name) == ts[0].indicators.end()) {
        indicators::add_zscore(ts, window, "close");
    }
    
    int current_position = 0;
    
    for (size_t i = 0; i < ts.size(); ++i) {
        auto& indicators = ts[i].indicators;
        
        if (indicators.find(zscore_name) != indicators.end()) {
            double z = indicators[zscore_name];
            
            if (!std::isnan(z)) {
                // Entry logic
                if (z < -entry_z) {
                    // Oversold - go long
                    current_position = 1;
                } else if (z > entry_z) {
                    // Overbought - go short
                    current_position = -1;
                }
                // Exit logic
                else if (std::abs(z) < exit_z && current_position != 0) {
                    // Return to mean - exit position
                    current_position = 0;
                }
                
                indicators[out_col] = static_cast<double>(current_position);
                ts[i].signal = current_position;
            } else {
                indicators[out_col] = 0.0;
                ts[i].signal = 0;
            }
        } else {
            indicators[out_col] = 0.0;
            ts[i].signal = 0;
        }
    }
}

void momentum_strategy(TimeSeries& ts, size_t window, double upper_threshold,
                      double lower_threshold, const std::string& col,
                      const std::string& out_col) {
    if (ts.size() <= window) return;
    
    for (size_t i = 0; i < ts.size(); ++i) {
        if (i < window) {
            ts[i].indicators[out_col] = 0.0;
            ts[i].signal = 0;
            continue;
        }
        
        double current_price = indicators::get_column_value(ts[i], col);
        double past_price = indicators::get_column_value(ts[i - window], col);
        
        if (past_price > 1e-10) {  // Avoid division by zero
            double momentum = (current_price - past_price) / past_price;
            
            int signal = 0;
            if (momentum > upper_threshold) {
                signal = 1;  // Long
            } else if (momentum < lower_threshold) {
                signal = -1;  // Short
            }
            
            ts[i].indicators[out_col] = static_cast<double>(signal);
            ts[i].signal = signal;
        } else {
            ts[i].indicators[out_col] = 0.0;
            ts[i].signal = 0;
        }
    }
}

void bollinger_breakout(TimeSeries& ts, size_t window, double num_std,
                       const std::string& col, const std::string& out_col) {
    if (ts.size() == 0) return;
    
    std::string mean_name = "ROLL_MEAN_" + std::to_string(window);
    std::string std_name = "ROLL_STD_" + std::to_string(window);
    
    // Ensure rolling mean and std are computed
    if (ts.size() > 0 && ts[0].indicators.find(mean_name) == ts[0].indicators.end()) {
        indicators::add_roll_mean_std(ts, window, col);
    }
    
    int current_position = 0;
    
    for (size_t i = 0; i < ts.size(); ++i) {
        auto& inds = ts[i].indicators;
        
        if (inds.find(mean_name) != inds.end() && inds.find(std_name) != inds.end()) {
            double mean = inds[mean_name];
            double sd = inds[std_name];
            double price = indicators::get_column_value(ts[i], col);
            
            if (!std::isnan(mean) && !std::isnan(sd)) {
                double upper_band = mean + num_std * sd;
                double lower_band = mean - num_std * sd;
                
                // Breakout above upper band - go long
                if (price > upper_band) {
                    current_position = 1;
                }
                // Breakout below lower band - go short
                else if (price < lower_band) {
                    current_position = -1;
                }
                // Return to within bands - exit
                else if (price >= lower_band && price <= upper_band && current_position != 0) {
                    current_position = 0;
                }
                
                inds[out_col] = static_cast<double>(current_position);
                ts[i].signal = current_position;
            } else {
                inds[out_col] = 0.0;
                ts[i].signal = 0;
            }
        } else {
            inds[out_col] = 0.0;
            ts[i].signal = 0;
        }
    }
}

} // namespace signals
} // namespace tsproc
