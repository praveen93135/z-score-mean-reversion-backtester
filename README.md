# Z-Score Mean Reversion Backtester

This project tests a z-score based mean-reversion strategy on Indian equities and ETFs.

## Strategy

The main strategy on `main` is a z-score dip-buying strategy with slope confirmation, a slower profit exit, and a protective stop.

- Compute a rolling moving average and rolling standard deviation.
- Compute z-score = `(price - moving average) / standard deviation`.
- Enter a long position when price is below the recent average and short-term slope has turned positive.
- Exit after the rebound weakens, or exit defensively if the trade falls too far.

Current preferred configuration:

```text
lookback = 60
entry z-score = -0.5
slope lookback = 7
slope exit mode = slow_stop
protective stop = 25% below entry
trend filter = off
transaction cost = 0.1%
```

With `slope-lookback = 7`, `slow_stop` exits if:

```text
the trade is profitable and the 14-day slope turns negative, or
the trade falls 25% below the entry price.
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
./build/backtester data/RELIANCE.csv 60 -0.5 0 0.001 7 slow_stop
```

You can also override strategy parameters:

```bash
./build/backtester <csv-file> <lookback> <entry-z> <exit-z> <transaction-cost> <slope-lookback> <slope-exit-mode> [trend-lookback]
```

Example:

```bash
./build/backtester data/RELIANCE.csv 60 -2 0 0.001 0
```

That example runs the original z-score-only baseline because `slope-lookback = 0`.

Set `slope-lookback` to a positive number to use slope confirmation:

```bash
./build/backtester data/RELIANCE.csv 60 -0.5 0 0.001 7 slow_stop
```

Slope exit modes:

```text
neg         -> exit when slope turns negative
profit      -> exit into strength using the positive-slope profit rules
slow_stop   -> exit when 14-day slope turns negative in profit, or stop at -25%
```

The optional `trend-lookback` argument enables an experimental trend filter. The current preferred strategy leaves this unset, so no SMA trend filter is applied.

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
./build/backtester data/<TICKER>.csv 60 -0.5 0 0.001 7 slow_stop
```

| Ticker | Strategy Return | Strategy Sharpe | Strategy Max DD | Trades | Buy & Hold Return | Buy & Hold Max DD |
|---|---:|---:|---:|---:|---:|---:|
| ADANIENT | 246.75% | 0.65 | -30.68% | 74 | 6805.16% | -71.35% |
| ADANIPORTS | 270.93% | 0.77 | -41.44% | 102 | 773.55% | -52.92% |
| APOLLOHOSP | 173.26% | 0.65 | -27.86% | 74 | 569.55% | -39.42% |
| ASIANPAINT | 167.88% | 0.77 | -18.32% | 78 | 183.38% | -39.04% |
| AXISBANK | 239.85% | 0.72 | -29.63% | 82 | 142.00% | -63.11% |
| BAJAJ-AUTO | 11.16% | 0.15 | -31.72% | 67 | 395.41% | -42.31% |
| BAJAJFINSV | 345.22% | 0.88 | -40.61% | 89 | 707.04% | -58.57% |
| BAJFINANCE | 248.00% | 0.67 | -29.21% | 67 | 1114.51% | -62.44% |
| BEL | 95.25% | 0.41 | -52.59% | 68 | 1181.24% | -67.20% |
| BHARTIARTL | 68.52% | 0.38 | -32.62% | 69 | 528.62% | -46.78% |
| CIPLA | 184.72% | 0.68 | -22.96% | 77 | 197.58% | -43.40% |
| COALINDIA | 92.45% | 0.44 | -28.19% | 71 | 189.10% | -58.80% |
| DRREDDY | 58.51% | 0.36 | -29.21% | 66 | 87.19% | -48.13% |
| EICHERMOT | 1.75% | 0.11 | -58.73% | 72 | 304.05% | -60.66% |
| ETERNAL | -57.43% | -0.46 | -65.39% | 38 | 132.10% | -74.02% |
| GRASIM | 80.29% | 0.42 | -44.25% | 48 | 379.91% | -68.59% |
| HCLTECH | 17.67% | 0.18 | -39.14% | 41 | 328.06% | -44.76% |
| HDFCBANK | 8.70% | 0.13 | -38.99% | 63 | 198.86% | -41.05% |
| HDFCLIFE | 4.58% | 0.12 | -42.83% | 79 | 64.48% | -46.19% |
| HINDALCO | 65.79% | 0.36 | -31.78% | 63 | 695.61% | -68.20% |
| HINDUNILVR | 68.69% | 0.43 | -24.01% | 53 | 171.01% | -30.46% |
| ICICIBANK | 192.83% | 0.71 | -23.44% | 59 | 537.82% | -48.31% |
| INDIGO | 187.92% | 0.60 | -46.36% | 107 | 447.81% | -54.65% |
| INFY | 37.79% | 0.28 | -35.31% | 77 | 136.62% | -48.17% |
| ITC | -13.00% | -0.01 | -46.68% | 67 | 68.70% | -55.28% |
| JIOFIN | -28.29% | -0.39 | -44.65% | 19 | -6.09% | -48.21% |
| JSWSTEEL | 252.03% | 0.81 | -43.91% | 90 | 805.27% | -65.74% |
| KOTAKBANK | 171.26% | 0.70 | -24.58% | 68 | 149.56% | -36.50% |
| LT | 155.40% | 0.67 | -34.67% | 84 | 341.17% | -54.44% |
| M&M | 80.46% | 0.41 | -37.74% | 79 | 364.01% | -72.28% |
| MARUTI | 64.55% | 0.37 | -40.59% | 75 | 251.96% | -58.26% |
| MAXHEALTH | 24.33% | 0.30 | -28.34% | 27 | 891.75% | -28.33% |
| NESTLEIND | 132.16% | 0.76 | -12.86% | 64 | 409.10% | -22.88% |
| NTPC | 99.71% | 0.53 | -31.37% | 83 | 277.27% | -46.83% |
| ONGC | 74.12% | 0.37 | -43.20% | 55 | 176.41% | -69.52% |
| POWERGRID | 83.64% | 0.47 | -27.54% | 65 | 375.04% | -29.56% |
| RELIANCE | 145.32% | 0.59 | -32.83% | 69 | 491.19% | -45.09% |
| SBILIFE | 102.52% | 0.55 | -30.96% | 43 | 162.24% | -46.84% |
| SBIN | 94.04% | 0.43 | -50.73% | 72 | 406.40% | -59.49% |
| SHRIRAMFIN | 149.33% | 0.48 | -49.40% | 101 | 376.92% | -71.87% |
| SUNPHARMA | 55.38% | 0.34 | -40.16% | 90 | 168.13% | -60.80% |
| TATACONSUM | 210.28% | 0.74 | -26.75% | 74 | 837.32% | -44.30% |
| TATASTEEL | 53.90% | 0.31 | -48.88% | 53 | 697.43% | -64.50% |
| TCS | 94.92% | 0.55 | -18.98% | 87 | 110.95% | -53.39% |
| TECHM | 66.00% | 0.40 | -31.48% | 88 | 298.82% | -46.57% |
| TITAN | 269.54% | 0.86 | -24.95% | 67 | 1073.22% | -41.73% |
| TMPV | -63.58% | -0.24 | -79.21% | 80 | -29.76% | -88.91% |
| TRENT | 93.98% | 0.42 | -51.25% | 56 | 1558.72% | -73.30% |
| ULTRACEMCO | 258.60% | 0.96 | -20.30% | 89 | 246.59% | -37.50% |
| WIPRO | 14.93% | 0.17 | -38.32% | 53 | 78.28% | -50.02% |

Average results:

| Metric | Average |
|---|---:|
| Stocks tested | 50 |
| Strategy return | 109.05% |
| Strategy annualized return | 6.41% |
| Strategy Sharpe | 0.44 |
| Strategy max drawdown | -36.51% |
| Trades | 69.64 |
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
