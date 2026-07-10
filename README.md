# Z-Score Mean Reversion Backtester

This project tests a z-score based mean-reversion strategy on Indian equities and ETFs.

## Strategy

This branch tests the positive-profit exit strategy with an extra `-5%` slope stop.

- Compute a rolling moving average and rolling standard deviation.
- Compute z-score = `(price - moving average) / standard deviation`.
- Enter a long position when price is below the recent average and short-term slope has turned positive.
- Exit into strength when the rebound is strong enough.
- Also exit if the last 7 trading days lost at least 5% and price is below its 60-day moving average.

Preferred configuration on this branch:

```text
lookback = 60
entry z-score = -1.0
slope lookback = 7
slope exit mode = profit_stop
transaction cost = 0.1%
```

With `slope-lookback = 7`, the positive-profit exit triggers if:

```text
last 7 trading days gained at least 3%, or
both 7-day blocks in the last 14 trading days gained at least 2%, or
all three 7-day blocks in the last 21 trading days gained at least 1%.
```

The stop exit triggers if:

```text
last 7 trading days lost at least 5%
and price is below its 60-day moving average.
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

Run this branch's preferred stop strategy:

```bash
./build/backtester data/RELIANCE.csv 60 -1.0 0 0.001 7 profit_stop
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
./build/backtester data/RELIANCE.csv 60 -1.0 0 0.001 7 profit_stop
```

Slope exit modes:

```text
neg         -> exit when slope turns negative
profit      -> exit into strength using the positive-slope profit rules
profit_stop -> profit rules plus a -5% 7-day negative-slope stop, confirmed by price below the 60-day moving average
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

Results below use this branch's preferred stop strategy:

```bash
./build/backtester data/<TICKER>.csv 60 -1.0 0 0.001 7 profit_stop
```

| Ticker | Strategy Return | Buy & Hold Return | Strategy Sharpe | Buy & Hold Sharpe | Strategy Max DD | Buy & Hold Max DD | Trades |
|---|---:|---:|---:|---:|---:|---:|---:|
| RELIANCE | 49.85% | 504.12% | 0.55 | 0.81 | -16.60% | -45.09% | 76 |
| TCS | 26.80% | 112.95% | 0.35 | 0.44 | -19.06% | -53.39% | 71 |
| INFY | 10.99% | 140.49% | 0.18 | 0.47 | -17.32% | -48.17% | 91 |
| HDFCBANK | 20.29% | 201.57% | 0.29 | 0.61 | -16.21% | -41.05% | 74 |
| ICICIBANK | 68.66% | 547.29% | 0.59 | 0.79 | -11.63% | -48.31% | 52 |
| SBIN | 45.17% | 413.28% | 0.38 | 0.68 | -12.63% | -59.49% | 60 |
| ITC | 33.44% | 68.52% | 0.41 | 0.34 | -14.71% | -55.28% | 76 |
| LT | 17.81% | 347.96% | 0.29 | 0.72 | -14.46% | -54.44% | 68 |
| AXISBANK | -13.16% | 147.06% | -0.13 | 0.45 | -20.57% | -63.11% | 76 |
| KOTAKBANK | 24.13% | 149.86% | 0.32 | 0.49 | -18.46% | -36.50% | 68 |
| HINDUNILVR | -8.13% | 171.78% | -0.09 | 0.57 | -25.35% | -30.46% | 64 |

Average strategy results across these stocks:

```text
Average return:       25.08%
Average Sharpe:        0.29
Average max drawdown: -17.00%
Average trades:       71
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
