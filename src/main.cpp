#include "csv_reader.hpp"
#include "timeseries.hpp"
#include "indicators.hpp"
#include "signals.hpp"
#include "io.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

namespace tsproc {

struct CLIConfig {
    std::string input_file;
    std::string output_file;
    std::vector<size_t> sma_windows;
    size_t zscore_window = 0;
    double zscore_entry = 2.0;
    double zscore_exit = 0.5;
    size_t fast_sma = 0;
    size_t slow_sma = 0;
    bool compute_rolling_stats = false;
    bool generate_sma_crossover = false;
    bool generate_zscore_signal = false;
    bool drop_na = true;
    bool binary_output = false;
    std::string mode = "batch"; // batch or stream
};

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n"
              << "High-Performance Time-Series Data Processor\n\n"
              << "Options:\n"
              << "  --input FILE          Input CSV file (required)\n"
              << "  --output FILE         Output CSV file (required)\n"
              << "  --sma N               Add SMA with window N (can specify multiple)\n"
              << "  --zwindow N           Compute rolling mean/std/zscore with window N\n"
              << "  --zentry THRESHOLD    Z-score entry threshold (default: 2.0)\n"
              << "  --zexit THRESHOLD     Z-score exit threshold (default: 0.5)\n"
              << "  --signal-z            Generate zscore mean reversion signal\n"
              << "  --fast-sma N          Fast SMA window for crossover\n"
              << "  --slow-sma N          Slow SMA window for crossover\n"
              << "  --signal-sma          Generate SMA crossover signal\n"
              << "  --binary              Output binary format in addition to CSV\n"
              << "  --keep-na             Keep NaN values (default: drop)\n"
              << "  --mode MODE           Processing mode: batch or stream (default: batch)\n"
              << "  --help                Show this help message\n\n"
              << "Examples:\n"
              << "  " << program_name << " --input data.csv --output out.csv --sma 20 --sma 50\n"
              << "  " << program_name << " --input data.csv --output out.csv --zwindow 20 --signal-z\n"
              << "  " << program_name << " --input data.csv --output out.csv --fast-sma 10 --slow-sma 50 --signal-sma\n";
}

bool parse_args(int argc, char* argv[], CLIConfig& config) {
    if (argc < 2) {
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            return false;
        }
        else if (arg == "--input" && i + 1 < argc) {
            config.input_file = argv[++i];
        }
        else if (arg == "--output" && i + 1 < argc) {
            config.output_file = argv[++i];
        }
        else if (arg == "--sma" && i + 1 < argc) {
            config.sma_windows.push_back(std::stoul(argv[++i]));
        }
        else if (arg == "--zwindow" && i + 1 < argc) {
            config.zscore_window = std::stoul(argv[++i]);
            config.compute_rolling_stats = true;
        }
        else if (arg == "--zentry" && i + 1 < argc) {
            config.zscore_entry = std::stod(argv[++i]);
        }
        else if (arg == "--zexit" && i + 1 < argc) {
            config.zscore_exit = std::stod(argv[++i]);
        }
        else if (arg == "--signal-z") {
            config.generate_zscore_signal = true;
        }
        else if (arg == "--fast-sma" && i + 1 < argc) {
            config.fast_sma = std::stoul(argv[++i]);
        }
        else if (arg == "--slow-sma" && i + 1 < argc) {
            config.slow_sma = std::stoul(argv[++i]);
        }
        else if (arg == "--signal-sma") {
            config.generate_sma_crossover = true;
        }
        else if (arg == "--binary") {
            config.binary_output = true;
        }
        else if (arg == "--keep-na") {
            config.drop_na = false;
        }
        else if (arg == "--mode" && i + 1 < argc) {
            config.mode = argv[++i];
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return false;
        }
    }
    
    // Validate required arguments
    if (config.input_file.empty() || config.output_file.empty()) {
        std::cerr << "Error: --input and --output are required\n";
        return false;
    }
    
    return true;
}

int run_cli(int argc, char* argv[]) {
    CLIConfig config;
    
    if (!parse_args(argc, argv, config)) {
        print_usage(argv[0]);
        return 1;
    }
    
    try {
        std::cout << "Loading data from: " << config.input_file << std::endl;
        
        // Read CSV
        CSVReader reader(config.input_file);
        TimeSeries ts = reader.read_to_timeseries(config.drop_na);
        
        std::cout << "Loaded " << ts.size() << " records" << std::endl;
        
        if (ts.size() == 0) {
            std::cerr << "Error: No data loaded from input file" << std::endl;
            return 1;
        }
        
        // Compute indicators
        for (size_t window : config.sma_windows) {
            std::cout << "Computing SMA(" << window << ")..." << std::endl;
            indicators::add_sma(ts, window, "close");
        }
        
        if (config.compute_rolling_stats && config.zscore_window > 0) {
            std::cout << "Computing rolling mean/std(" << config.zscore_window << ")..." << std::endl;
            indicators::add_roll_mean_std(ts, config.zscore_window, "close");
            
            std::cout << "Computing Z-score(" << config.zscore_window << ")..." << std::endl;
            indicators::add_zscore(ts, config.zscore_window, "close");
        }
        
        // Generate signals
        if (config.generate_sma_crossover && config.fast_sma > 0 && config.slow_sma > 0) {
            std::cout << "Generating SMA crossover signal (fast=" << config.fast_sma 
                      << ", slow=" << config.slow_sma << ")..." << std::endl;
            signals::sma_crossover(ts, config.fast_sma, config.slow_sma, "signal_sma");
        }
        
        if (config.generate_zscore_signal && config.zscore_window > 0) {
            std::cout << "Generating Z-score mean reversion signal (entry=" 
                      << config.zscore_entry << ", exit=" << config.zscore_exit << ")..." << std::endl;
            signals::zscore_mean_reversion(ts, config.zscore_window, 
                                          config.zscore_entry, config.zscore_exit, "signal_z");
        }
        
        // Write output
        std::cout << "Writing output to: " << config.output_file << std::endl;
        CSVWriter writer(config.output_file);
        writer.write(ts);
        
        if (config.binary_output) {
            std::string binary_path = config.output_file + ".bin";
            std::cout << "Writing binary output to: " << binary_path << std::endl;
            BinaryWriter bin_writer(binary_path);
            bin_writer.write(ts);
        }
        
        std::cout << "Processing complete!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

} // namespace tsproc

int main(int argc, char* argv[]) {
    return tsproc::run_cli(argc, argv);
}
