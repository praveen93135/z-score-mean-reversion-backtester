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

Results below use the current preferred `main` strategy:

```bash
./build/backtester data/<TICKER>.csv 60 -1.0 0 0.001 7 profit
```

| Ticker | Strategy Return | Buy & Hold Return | Strategy Sharpe | Buy & Hold Sharpe | Strategy Max DD | Buy & Hold Max DD | Trades |
|---|---:|---:|---:|---:|---:|---:|---:|
| RELIANCE | 67.74% | 504.12% | 0.64 | 0.81 | -12.19% | -45.09% | 70 |
| TCS | 54.32% | 112.95% | 0.56 | 0.44 | -12.06% | -53.39% | 65 |
| INFY | 15.76% | 140.49% | 0.22 | 0.47 | -21.03% | -48.17% | 85 |
| HDFCBANK | 30.62% | 201.57% | 0.35 | 0.61 | -22.42% | -41.05% | 62 |
| ICICIBANK | 96.23% | 547.29% | 0.74 | 0.79 | -8.07% | -48.31% | 48 |
| SBIN | 85.21% | 413.28% | 0.56 | 0.68 | -12.78% | -59.49% | 48 |
| ITC | 1.62% | 68.52% | 0.07 | 0.34 | -37.54% | -55.28% | 70 |
| LT | 30.76% | 347.96% | 0.39 | 0.72 | -11.31% | -54.44% | 58 |
| AXISBANK | 1.47% | 147.06% | 0.06 | 0.45 | -16.49% | -63.11% | 62 |
| KOTAKBANK | 50.78% | 149.86% | 0.50 | 0.49 | -20.50% | -36.50% | 62 |
| HINDUNILVR | -0.88% | 171.78% | 0.03 | 0.57 | -24.64% | -30.46% | 54 |

Average strategy results across these stocks:

```text
Average return:       39.42%
Average Sharpe:        0.37
Average max drawdown: -18.09%
Average trades:       62
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
