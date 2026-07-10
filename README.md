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
