# Z-Score Mean Reversion Backtester

This project tests a z-score based mean-reversion strategy on Indian equities and ETFs.

## Strategy

The main strategy on `main` is a z-score dip-buying strategy with slope confirmation and a positive-profit exit.

- Compute a rolling moving average and rolling standard deviation.
- Compute z-score = `(price - moving average) / standard deviation`.
- Enter a long position when price is below the recent average and short-term slope has turned positive.
- Exit into strength when the rebound is strong enough.

Current preferred configuration:

```text
lookback = 60
entry z-score = -1.0
slope lookback = 7
slope exit mode = profit
transaction cost = 0.1%
```

With `slope-lookback = 7`, the positive-profit exit triggers if:

```text
last 7 trading days gained at least 3%, or
both 7-day blocks in the last 14 trading days gained at least 2%, or
all three 7-day blocks in the last 21 trading days gained at least 1%.
```

The original simple strategy is still available by setting `slope-lookback = 0`:

```text
Enter when z-score < entry-z.
Exit when z-score > exit-z.
```

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

Run the current preferred strategy:

```bash
./build/backtester data/RELIANCE.csv 60 -1.0 0 0.001 7 profit
```

You can also override strategy parameters:

```bash
./build/backtester <csv-file> <lookback> <entry-z> <exit-z> <transaction-cost> <slope-lookback> <slope-exit-mode>
```

Example:

```bash
./build/backtester data/RELIANCE.csv 60 -2 0 0.001 0
```

That example runs the original z-score-only baseline because `slope-lookback = 0`.

Set `slope-lookback` to a positive number to use slope confirmation:

```bash
./build/backtester data/RELIANCE.csv 60 -1.0 0 0.001 7 profit
```

Slope exit modes:

```text
neg         -> exit when slope turns negative
profit      -> exit into strength using the positive-slope profit rules
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

## Results

Results below use the current preferred `main` strategy on the 50 downloaded NIFTY stock CSVs, excluding `NIFTYBEES`, `BANKBEES`, and `sample_prices`:

```bash
./build/backtester data/<TICKER>.csv 60 -1.0 0 0.001 7 profit
```

| Ticker | Strategy Return | Strategy Sharpe | Strategy Max DD | Trades | Buy & Hold Return | Buy & Hold Max DD |
|---|---:|---:|---:|---:|---:|---:|
| ADANIENT | 105.40% | 0.54 | -33.57% | 70 | 6805.16% | -71.35% |
| ADANIPORTS | -17.97% | -0.02 | -54.39% | 80 | 773.55% | -52.92% |
| APOLLOHOSP | 103.19% | 0.92 | -9.46% | 72 | 569.55% | -39.42% |
| ASIANPAINT | -8.20% | -0.05 | -28.44% | 52 | 183.38% | -39.04% |
| AXISBANK | 1.47% | 0.06 | -16.49% | 62 | 142.00% | -63.11% |
| BAJAJ-AUTO | 7.77% | 0.15 | -16.77% | 44 | 395.41% | -42.31% |
| BAJAJFINSV | 52.41% | 0.49 | -26.61% | 66 | 707.04% | -58.57% |
| BAJFINANCE | 54.08% | 0.41 | -22.89% | 76 | 1114.51% | -62.44% |
| BEL | 19.00% | 0.19 | -40.52% | 76 | 1181.24% | -67.20% |
| BHARTIARTL | 19.33% | 0.22 | -24.49% | 50 | 528.62% | -46.78% |
| CIPLA | -27.98% | -0.26 | -41.24% | 56 | 197.58% | -43.40% |
| COALINDIA | -24.13% | -0.19 | -38.15% | 59 | 189.10% | -58.80% |
| DRREDDY | 22.99% | 0.30 | -18.79% | 60 | 87.19% | -48.13% |
| EICHERMOT | 47.33% | 0.42 | -21.30% | 76 | 304.05% | -60.66% |
| ETERNAL | 32.42% | 0.40 | -25.80% | 48 | 132.10% | -74.02% |
| GRASIM | 14.32% | 0.20 | -13.33% | 64 | 379.91% | -68.59% |
| HCLTECH | 13.66% | 0.17 | -25.41% | 62 | 328.06% | -44.76% |
| HDFCBANK | 30.62% | 0.35 | -22.42% | 62 | 198.86% | -41.05% |
| HDFCLIFE | -30.66% | -0.19 | -54.61% | 58 | 64.48% | -46.19% |
| HINDALCO | 23.60% | 0.24 | -29.60% | 59 | 695.61% | -68.20% |
| HINDUNILVR | -0.88% | 0.03 | -24.64% | 54 | 171.01% | -30.46% |
| ICICIBANK | 96.23% | 0.74 | -8.07% | 48 | 537.82% | -48.31% |
| INDIGO | 127.76% | 0.71 | -19.25% | 96 | 447.81% | -54.65% |
| INFY | 14.01% | 0.20 | -21.03% | 84 | 136.62% | -48.17% |
| ITC | 1.62% | 0.07 | -37.54% | 70 | 68.70% | -55.28% |
| JIOFIN | -33.69% | -0.77 | -44.78% | 34 | -6.09% | -48.21% |
| JSWSTEEL | 1.96% | 0.07 | -29.72% | 58 | 805.27% | -65.74% |
| KOTAKBANK | 50.78% | 0.50 | -20.50% | 62 | 149.56% | -36.50% |
| LT | 30.76% | 0.39 | -11.31% | 58 | 341.17% | -54.44% |
| M&M | 65.82% | 0.49 | -24.54% | 60 | 364.01% | -72.28% |
| MARUTI | 35.24% | 0.34 | -23.05% | 56 | 251.96% | -58.26% |
| MAXHEALTH | 10.12% | 0.24 | -11.24% | 16 | 891.75% | -28.33% |
| NESTLEIND | 76.80% | 0.83 | -7.92% | 60 | 409.10% | -22.88% |
| NTPC | -9.24% | -0.04 | -32.35% | 65 | 277.27% | -46.83% |
| ONGC | -22.06% | -0.20 | -38.83% | 69 | 176.41% | -69.52% |
| POWERGRID | 31.30% | 0.38 | -15.87% | 57 | 375.04% | -29.56% |
| RELIANCE | 67.74% | 0.64 | -12.19% | 70 | 491.19% | -45.09% |
| SBILIFE | -25.98% | -0.21 | -42.53% | 48 | 162.24% | -46.84% |
| SBIN | 85.21% | 0.56 | -12.78% | 48 | 406.40% | -59.49% |
| SHRIRAMFIN | 65.75% | 0.41 | -34.04% | 98 | 376.92% | -71.87% |
| SUNPHARMA | 59.93% | 0.57 | -15.98% | 88 | 168.13% | -60.80% |
| TATACONSUM | 29.18% | 0.34 | -15.70% | 74 | 837.32% | -44.30% |
| TATASTEEL | -0.92% | 0.05 | -33.31% | 77 | 697.43% | -64.50% |
| TCS | 52.87% | 0.54 | -12.06% | 65 | 110.95% | -53.39% |
| TECHM | -0.52% | 0.05 | -34.89% | 60 | 298.82% | -46.57% |
| TITAN | 34.47% | 0.37 | -18.37% | 66 | 1073.22% | -41.73% |
| TMPV | 15.23% | 0.17 | -40.27% | 100 | -29.76% | -88.91% |
| TRENT | 46.96% | 0.49 | -16.82% | 56 | 1558.72% | -73.30% |
| ULTRACEMCO | 60.98% | 0.62 | -16.29% | 70 | 246.59% | -37.50% |
| WIPRO | -8.15% | -0.05 | -22.33% | 65 | 78.28% | -50.02% |

Average results:

| Metric | Average |
|---|---:|
| Stocks tested | 50 |
| Strategy return | 27.96% |
| Strategy annualized return | 1.99% |
| Strategy Sharpe | 0.26 |
| Strategy max drawdown | -25.25% |
| Trades | 64 |
| Buy-and-hold return | 537.03% |
| Buy-and-hold max drawdown | -53.01% |

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
