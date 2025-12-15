#!/usr/bin/env python3
"""
Generate synthetic OHLCV time-series data for testing
"""
import csv
import random
from datetime import datetime, timedelta

def generate_ohlcv(n_rows=10000, start_price=100.0, volatility=0.02, trend=0.0001):
    """
    Generate synthetic OHLCV data with realistic price movements
    
    Args:
        n_rows: Number of rows to generate
        start_price: Starting price
        volatility: Daily volatility (standard deviation of returns)
        trend: Daily drift (mean return)
    """
    data = []
    current_price = start_price
    start_date = datetime(2020, 1, 1)
    
    for i in range(n_rows):
        # Generate date
        date = start_date + timedelta(days=i)
        date_str = date.strftime("%Y-%m-%d")
        
        # Random daily return
        daily_return = random.gauss(trend, volatility)
        
        # Open price (previous close with small gap)
        open_price = current_price * (1 + random.gauss(0, volatility * 0.2))
        
        # Close price (apply daily return)
        close_price = open_price * (1 + daily_return)
        
        # High and low (based on intraday range)
        intraday_range = abs(random.gauss(0, volatility * 0.5))
        high_price = max(open_price, close_price) * (1 + intraday_range)
        low_price = min(open_price, close_price) * (1 - intraday_range)
        
        # Volume (random around a mean)
        volume = int(random.gauss(1000000, 200000))
        
        data.append({
            'Date': date_str,
            'Open': round(open_price, 2),
            'High': round(high_price, 2),
            'Low': round(low_price, 2),
            'Close': round(close_price, 2),
            'Adj Close': round(close_price, 2),
            'Volume': max(volume, 100000)  # Ensure positive volume
        })
        
        current_price = close_price
    
    return data

def write_csv(data, filename):
    """Write data to CSV file"""
    fieldnames = ['Date', 'Open', 'High', 'Low', 'Close', 'Adj Close', 'Volume']
    
    with open(filename, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(data)
    
    print(f"Generated {len(data)} rows -> {filename}")

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description='Generate synthetic OHLCV data')
    parser.add_argument('-n', '--rows', type=int, default=10000, help='Number of rows')
    parser.add_argument('-o', '--output', default='../data/synthetic.csv', help='Output file')
    parser.add_argument('--start-price', type=float, default=100.0, help='Starting price')
    parser.add_argument('--volatility', type=float, default=0.02, help='Daily volatility')
    parser.add_argument('--trend', type=float, default=0.0001, help='Daily trend')
    
    args = parser.parse_args()
    
    data = generate_ohlcv(
        n_rows=args.rows,
        start_price=args.start_price,
        volatility=args.volatility,
        trend=args.trend
    )
    
    write_csv(data, args.output)
