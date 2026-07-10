# Z-Score Mean Reversion Backtester

This project tests a z-score based mean-reversion strategy on Indian equities and ETFs.

## Strategy

- Compute a 20-day moving average and rolling standard deviation.
- Compute z-score = `(price - moving average) / standard deviation`.
- Enter a long position when z-score `< -2`.
- Exit when z-score `> 0`.

## Backtesting Assumptions

- Daily closing prices.
- Long-only strategy.
- Signal is shifted by one day to avoid lookahead bias.
- Transaction cost of 0.1% per trade.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/backtester data/sample_prices.csv
```

You can also override strategy parameters:

```bash
./build/backtester <csv-file> <lookback> <entry-z> <exit-z> <transaction-cost> <slope-lookback>
```

Example:

```bash
./build/backtester data/RELIANCE.csv 60 -2 0 0.001
```

Set `slope-lookback` to a positive number to use slope confirmation.
For entry, the short slope must be positive.
For exit, both the short slope and `2 * slope-lookback` slope must be negative.

```bash
./build/backtester data/RELIANCE.csv 20 -1.0 0 0.001 7
```

The program prints a metrics table and writes daily strategy details to:

```text
results/daily_results.csv
results/trades.csv
```

The input CSV should contain `Date` and either `Adj Close` or `Close`.
If both are available, the backtester uses `Adj Close` because it is better for historical return calculations.

```csv
Date,Close
2024-01-01,100.50
```

## Download Data

Yahoo Finance may not always show a visible CSV download button. As an alternative, install the optional Python downloader dependency:

```bash
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
```

Then download default sample tickers:

```bash
.venv/bin/python scripts/download_yahoo_data.py
```

This writes:

```text
data/RELIANCE.csv
data/NIFTYBEES.csv
data/BANKBEES.csv
```

You can also pass custom Yahoo tickers:

```bash
.venv/bin/python scripts/download_yahoo_data.py TCS.NS INFY.NS HDFCBANK.NS
```

Check downloaded data for suspicious one-day moves:

```bash
.venv/bin/python scripts/check_data_quality.py
```

## Metrics

- Total return
- Annualized return
- Sharpe ratio
- Maximum drawdown
- Number of trades
- Buy-and-hold comparison

## Limitations

- Uses daily data only.
- Does not deeply model liquidity or slippage.
- Strategy may be overfit to selected parameters.
- Past performance does not imply future returns.
