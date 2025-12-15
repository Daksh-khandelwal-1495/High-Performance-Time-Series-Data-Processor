#include <gtest/gtest.h>
#include "io.hpp"
#include "timeseries.hpp"
#include "indicators.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

class IOTest : public ::testing::Test {
protected:
    std::string test_output_path;
    
    void SetUp() override {
        test_output_path = "test_output.csv";
    }
    
    void TearDown() override {
        if (fs::exists(test_output_path)) {
            fs::remove(test_output_path);
        }
        if (fs::exists(test_output_path + ".bin")) {
            fs::remove(test_output_path + ".bin");
        }
    }
    
    tsproc::TimeSeries create_test_series() {
        tsproc::TimeSeries ts;
        for (int i = 0; i < 5; ++i) {
            tsproc::Record r;
            r.date = "2020-01-0" + std::to_string(i + 1);
            r.open = 100.0 + i;
            r.high = 105.0 + i;
            r.low = 99.0 + i;
            r.close = 103.0 + i;
            r.adj_close = 103.0 + i;
            r.volume = 1000000.0 + i * 10000;
            ts.push(r);
        }
        return ts;
    }
    
    std::string read_file_content(const std::string& path) {
        std::ifstream ifs(path);
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        return buffer.str();
    }
};

TEST_F(IOTest, CSVWriter_BasicWrite) {
    tsproc::TimeSeries ts = create_test_series();
    
    tsproc::CSVWriter writer(test_output_path);
    writer.write(ts);
    
    EXPECT_TRUE(fs::exists(test_output_path));
    
    // Read and verify content
    std::string content = read_file_content(test_output_path);
    EXPECT_FALSE(content.empty());
    EXPECT_TRUE(content.find("Date,Open,High,Low,Close") != std::string::npos);
    EXPECT_TRUE(content.find("2020-01-01") != std::string::npos);
}

TEST_F(IOTest, CSVWriter_WithIndicators) {
    tsproc::TimeSeries ts = create_test_series();
    
    // Add indicators
    tsproc::indicators::add_sma(ts, 3, "close");
    
    tsproc::CSVWriter writer(test_output_path);
    writer.write(ts);
    
    std::string content = read_file_content(test_output_path);
    
    // Should contain SMA_3 column
    EXPECT_TRUE(content.find("SMA_3") != std::string::npos);
}

TEST_F(IOTest, CSVWriter_MultipleIndicators) {
    tsproc::TimeSeries ts = create_test_series();
    
    tsproc::indicators::add_sma(ts, 2, "close");
    tsproc::indicators::add_sma(ts, 3, "close");
    tsproc::indicators::add_zscore(ts, 3, "close");
    
    tsproc::CSVWriter writer(test_output_path);
    writer.write(ts);
    
    std::string content = read_file_content(test_output_path);
    
    EXPECT_TRUE(content.find("SMA_2") != std::string::npos);
    EXPECT_TRUE(content.find("SMA_3") != std::string::npos);
    EXPECT_TRUE(content.find("ZSCORE_3") != std::string::npos);
}

TEST_F(IOTest, CSVWriter_NaNHandling) {
    tsproc::TimeSeries ts = create_test_series();
    
    // Add SMA which will have NaN values at the start
    tsproc::indicators::add_sma(ts, 3, "close");
    
    tsproc::CSVWriter writer(test_output_path);
    writer.write(ts);
    
    std::string content = read_file_content(test_output_path);
    
    // Should contain "NaN" for missing values
    EXPECT_TRUE(content.find("NaN") != std::string::npos);
}

TEST_F(IOTest, BinaryWriter_BasicWrite) {
    tsproc::TimeSeries ts = create_test_series();
    
    tsproc::CSVWriter writer(test_output_path);
    std::string binary_path = test_output_path + ".bin";
    writer.write_binary(ts, binary_path);
    
    EXPECT_TRUE(fs::exists(binary_path));
    
    // Check file is not empty
    EXPECT_GT(fs::file_size(binary_path), 0);
}

TEST_F(IOTest, BinaryWriter_WithIndicators) {
    tsproc::TimeSeries ts = create_test_series();
    
    tsproc::indicators::add_sma(ts, 3, "close");
    
    tsproc::CSVWriter writer(test_output_path);
    std::string binary_path = test_output_path + ".bin";
    writer.write_binary(ts, binary_path);
    
    EXPECT_TRUE(fs::exists(binary_path));
    EXPECT_GT(fs::file_size(binary_path), 0);
}

TEST_F(IOTest, WriteEmptyTimeSeries) {
    tsproc::TimeSeries ts;
    
    tsproc::CSVWriter writer(test_output_path);
    
    // Should throw or handle gracefully
    EXPECT_THROW(writer.write(ts), std::runtime_error);
}

TEST_F(IOTest, OutputFormatConsistency) {
    tsproc::TimeSeries ts = create_test_series();
    
    tsproc::CSVWriter writer(test_output_path);
    writer.write(ts);
    
    std::ifstream ifs(test_output_path);
    std::string header;
    std::getline(ifs, header);
    
    // Count columns in header
    int header_cols = std::count(header.begin(), header.end(), ',') + 1;
    
    // Read first data line
    std::string first_line;
    std::getline(ifs, first_line);
    
    // Count columns in data
    int data_cols = std::count(first_line.begin(), first_line.end(), ',') + 1;
    
    // Should match
    EXPECT_EQ(header_cols, data_cols);
}
