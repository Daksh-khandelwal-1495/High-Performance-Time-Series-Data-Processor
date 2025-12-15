#include <gtest/gtest.h>
#include "csv_reader.hpp"
#include "timeseries.hpp"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class CSVReaderTest : public ::testing::Test {
protected:
    std::string test_csv_path;
    
    void SetUp() override {
        test_csv_path = "test_data.csv";
    }
    
    void TearDown() override {
        if (fs::exists(test_csv_path)) {
            fs::remove(test_csv_path);
        }
    }
    
    void create_test_csv(const std::string& content) {
        std::ofstream ofs(test_csv_path);
        ofs << content;
        ofs.close();
    }
};

TEST_F(CSVReaderTest, BasicParsing) {
    std::string content = 
        "Date,Open,High,Low,Close,Adj Close,Volume\n"
        "2020-01-01,100.0,105.0,99.0,103.0,103.0,1000000\n"
        "2020-01-02,103.0,107.0,102.0,106.0,106.0,1100000\n";
    
    create_test_csv(content);
    
    tsproc::CSVReader reader(test_csv_path);
    tsproc::TimeSeries ts = reader.read_to_timeseries(false);
    
    ASSERT_EQ(ts.size(), 2);
    EXPECT_EQ(ts[0].date, "2020-01-01");
    EXPECT_DOUBLE_EQ(ts[0].open, 100.0);
    EXPECT_DOUBLE_EQ(ts[0].high, 105.0);
    EXPECT_DOUBLE_EQ(ts[0].low, 99.0);
    EXPECT_DOUBLE_EQ(ts[0].close, 103.0);
    EXPECT_DOUBLE_EQ(ts[0].volume, 1000000.0);
}

TEST_F(CSVReaderTest, EmptyFile) {
    create_test_csv("Date,Open,High,Low,Close,Adj Close,Volume\n");
    
    tsproc::CSVReader reader(test_csv_path);
    tsproc::TimeSeries ts = reader.read_to_timeseries(false);
    
    EXPECT_EQ(ts.size(), 0);
}

TEST_F(CSVReaderTest, MissingValues) {
    std::string content = 
        "Date,Open,High,Low,Close,Adj Close,Volume\n"
        "2020-01-01,100.0,105.0,99.0,103.0,103.0,1000000\n"
        "2020-01-02,,,,,, \n"
        "2020-01-03,110.0,115.0,109.0,113.0,113.0,1200000\n";
    
    create_test_csv(content);
    
    tsproc::CSVReader reader(test_csv_path);
    
    // With drop_na = true
    tsproc::TimeSeries ts_drop = reader.read_to_timeseries(true);
    EXPECT_EQ(ts_drop.size(), 2);
    
    // With drop_na = false
    tsproc::CSVReader reader2(test_csv_path);
    tsproc::TimeSeries ts_keep = reader2.read_to_timeseries(false);
    EXPECT_EQ(ts_keep.size(), 3);
}

TEST_F(CSVReaderTest, DifferentDelimiter) {
    std::string content = 
        "Date;Open;High;Low;Close;Adj Close;Volume\n"
        "2020-01-01;100.0;105.0;99.0;103.0;103.0;1000000\n";
    
    create_test_csv(content);
    
    tsproc::CSVReader reader(test_csv_path, ';');
    tsproc::TimeSeries ts = reader.read_to_timeseries(false);
    
    ASSERT_EQ(ts.size(), 1);
    EXPECT_DOUBLE_EQ(ts[0].close, 103.0);
}

TEST_F(CSVReaderTest, CaseInsensitiveHeaders) {
    std::string content = 
        "date,OPEN,high,LOW,Close,adj close,VOLUME\n"
        "2020-01-01,100.0,105.0,99.0,103.0,103.0,1000000\n";
    
    create_test_csv(content);
    
    tsproc::CSVReader reader(test_csv_path);
    tsproc::TimeSeries ts = reader.read_to_timeseries(false);
    
    ASSERT_EQ(ts.size(), 1);
    EXPECT_DOUBLE_EQ(ts[0].open, 100.0);
}

TEST_F(CSVReaderTest, GetCloseSeries) {
    std::string content = 
        "Date,Open,High,Low,Close,Adj Close,Volume\n"
        "2020-01-01,100.0,105.0,99.0,103.0,103.0,1000000\n"
        "2020-01-02,103.0,107.0,102.0,106.0,106.0,1100000\n"
        "2020-01-03,106.0,110.0,105.0,109.0,109.0,1200000\n";
    
    create_test_csv(content);
    
    tsproc::CSVReader reader(test_csv_path);
    tsproc::TimeSeries ts = reader.read_to_timeseries(false);
    
    std::vector<double> close_series = ts.get_close_series();
    
    ASSERT_EQ(close_series.size(), 3);
    EXPECT_DOUBLE_EQ(close_series[0], 103.0);
    EXPECT_DOUBLE_EQ(close_series[1], 106.0);
    EXPECT_DOUBLE_EQ(close_series[2], 109.0);
}
