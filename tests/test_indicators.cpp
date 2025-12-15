#include <gtest/gtest.h>
#include "indicators.hpp"
#include "timeseries.hpp"
#include <cmath>
#include <vector>

class IndicatorsTest : public ::testing::Test {
protected:
    tsproc::TimeSeries create_simple_series(const std::vector<double>& close_prices) {
        tsproc::TimeSeries ts;
        for (size_t i = 0; i < close_prices.size(); ++i) {
            tsproc::Record r;
            r.date = "2020-01-" + std::to_string(i + 1);
            r.open = close_prices[i];
            r.high = close_prices[i] + 1.0;
            r.low = close_prices[i] - 1.0;
            r.close = close_prices[i];
            r.adj_close = close_prices[i];
            r.volume = 1000000.0;
            ts.push(r);
        }
        return ts;
    }
};

TEST_F(IndicatorsTest, SMA_BasicCalculation) {
    // Test data: [1,2,3,4,5,6,7,8,9,10]
    // Expected SMA_3: NaN, NaN, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0
    std::vector<double> prices = {1,2,3,4,5,6,7,8,9,10};
    tsproc::TimeSeries ts = create_simple_series(prices);
    
    tsproc::indicators::add_sma(ts, 3, "close");
    
    ASSERT_EQ(ts.size(), 10);
    
    // First two should be NaN
    EXPECT_TRUE(std::isnan(ts[0].indicators["SMA_3"]));
    EXPECT_TRUE(std::isnan(ts[1].indicators["SMA_3"]));
    
    // Check calculated values
    EXPECT_DOUBLE_EQ(ts[2].indicators["SMA_3"], 2.0);  // (1+2+3)/3
    EXPECT_DOUBLE_EQ(ts[3].indicators["SMA_3"], 3.0);  // (2+3+4)/3
    EXPECT_DOUBLE_EQ(ts[4].indicators["SMA_3"], 4.0);  // (3+4+5)/3
    EXPECT_DOUBLE_EQ(ts[9].indicators["SMA_3"], 9.0);  // (8+9+10)/3
}

TEST_F(IndicatorsTest, SMA_Window5) {
    std::vector<double> prices = {10, 20, 30, 40, 50, 60};
    tsproc::TimeSeries ts = create_simple_series(prices);
    
    tsproc::indicators::add_sma(ts, 5, "close");
    
    // First 4 should be NaN
    for (int i = 0; i < 4; ++i) {
        EXPECT_TRUE(std::isnan(ts[i].indicators["SMA_5"]));
    }
    
    // 5th element: (10+20+30+40+50)/5 = 30
    EXPECT_DOUBLE_EQ(ts[4].indicators["SMA_5"], 30.0);
    
    // 6th element: (20+30+40+50+60)/5 = 40
    EXPECT_DOUBLE_EQ(ts[5].indicators["SMA_5"], 40.0);
}

TEST_F(IndicatorsTest, SMA_EmptySeries) {
    tsproc::TimeSeries ts;
    tsproc::indicators::add_sma(ts, 3, "close");
    EXPECT_EQ(ts.size(), 0);
}

TEST_F(IndicatorsTest, SMA_WindowTooLarge) {
    std::vector<double> prices = {1, 2, 3};
    tsproc::TimeSeries ts = create_simple_series(prices);
    
    tsproc::indicators::add_sma(ts, 10, "close");
    
    // All should be NaN
    for (size_t i = 0; i < ts.size(); ++i) {
        EXPECT_TRUE(std::isnan(ts[i].indicators["SMA_10"]));
    }
}

TEST_F(IndicatorsTest, RollingMeanStd_BasicCalculation) {
    std::vector<double> prices = {1, 2, 3, 4, 5};
    tsproc::TimeSeries ts = create_simple_series(prices);
    
    tsproc::indicators::add_roll_mean_std(ts, 3, "close");
    
    // First two should be NaN
    EXPECT_TRUE(std::isnan(ts[0].indicators["ROLL_MEAN_3"]));
    EXPECT_TRUE(std::isnan(ts[1].indicators["ROLL_MEAN_3"]));
    
    // Third element: mean of [1,2,3] = 2.0
    EXPECT_DOUBLE_EQ(ts[2].indicators["ROLL_MEAN_3"], 2.0);
    
    // Standard deviation of [1,2,3]
    // variance = ((1-2)^2 + (2-2)^2 + (3-2)^2) / 3 = 2/3 ≈ 0.6667
    // std = sqrt(0.6667) ≈ 0.8165
    EXPECT_NEAR(ts[2].indicators["ROLL_STD_3"], 0.8165, 0.001);
}

TEST_F(IndicatorsTest, RollingMeanStd_ConstantValues) {
    std::vector<double> prices = {5, 5, 5, 5, 5};
    tsproc::TimeSeries ts = create_simple_series(prices);
    
    tsproc::indicators::add_roll_mean_std(ts, 3, "close");
    
    // Mean should be 5.0
    EXPECT_DOUBLE_EQ(ts[2].indicators["ROLL_MEAN_3"], 5.0);
    
    // Std should be 0.0 (no variation)
    EXPECT_DOUBLE_EQ(ts[2].indicators["ROLL_STD_3"], 0.0);
}

TEST_F(IndicatorsTest, ZScore_BasicCalculation) {
    std::vector<double> prices = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tsproc::TimeSeries ts = create_simple_series(prices);
    
    tsproc::indicators::add_zscore(ts, 3, "close");
    
    // First two should be NaN
    EXPECT_TRUE(std::isnan(ts[0].indicators["ZSCORE_3"]));
    EXPECT_TRUE(std::isnan(ts[1].indicators["ZSCORE_3"]));
    
    // Third element: values [1,2,3], mean=2, std≈0.8165
    // z-score for 3 = (3-2)/0.8165 ≈ 1.225
    EXPECT_NEAR(ts[2].indicators["ZSCORE_3"], 1.225, 0.01);
}

TEST_F(IndicatorsTest, ZScore_OutlierDetection) {
    std::vector<double> prices = {10, 10, 10, 10, 20}; // Last value is outlier
    tsproc::TimeSeries ts = create_simple_series(prices);
    
    tsproc::indicators::add_zscore(ts, 3, "close");
    
    // Last element: values [10,10,20], mean≈13.33, value=20
    // Should have positive z-score
    EXPECT_GT(ts[4].indicators["ZSCORE_3"], 1.0);
}

TEST_F(IndicatorsTest, MultipleIndicators) {
    std::vector<double> prices = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tsproc::TimeSeries ts = create_simple_series(prices);
    
    tsproc::indicators::add_sma(ts, 3, "close");
    tsproc::indicators::add_sma(ts, 5, "close");
    tsproc::indicators::add_zscore(ts, 3, "close");
    
    // Check that all indicators exist
    EXPECT_TRUE(ts[5].indicators.find("SMA_3") != ts[5].indicators.end());
    EXPECT_TRUE(ts[5].indicators.find("SMA_5") != ts[5].indicators.end());
    EXPECT_TRUE(ts[5].indicators.find("ZSCORE_3") != ts[5].indicators.end());
}
