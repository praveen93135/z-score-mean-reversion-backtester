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

The program prints a metrics table and writes daily strategy details to:

```text
results/daily_results.csv
results/trades.csv
```

The input CSV should contain at least these columns:

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
