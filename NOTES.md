# Project Notes

This file collects design questions, answers, and trading/backtesting ideas that come up while building the project.

## Why is `date` stored as a string?

For version 1, the date is only used as a label for each price row.

Example:

```text
2024-01-01 -> close = 100.0
2024-01-02 -> close = 101.0
```

The strategy uses the order of rows and the closing prices. It does not currently calculate calendar differences, weekends, holidays, monthly returns, or yearly reports. Because of that, `std::string` is simple and enough.

Later, if we need date arithmetic or multi-stock date alignment, we can upgrade this to a proper C++ date type.

## Why sort the CSV by date?

The backtester needs prices in oldest-to-newest order because returns are calculated like this:

```cpp
todayReturn = todayClose / yesterdayClose - 1.0;
```

If rows are newest-to-oldest, the return calculation becomes wrong.

Because the CSV date format is `YYYY-MM-DD`, normal string sorting matches chronological sorting:

```text
2024-01-01
2024-02-15
2025-01-01
```

This works because the biggest unit comes first: year, then month, then day.

Important limitation: string sorting is safe only if dates are consistently formatted as `YYYY-MM-DD`.

## What is `close`?

`close` is the closing price of the stock or ETF for that trading day.

A daily market candle often has:

```text
Open   = price when market opened
High   = highest price during the day
Low    = lowest price during the day
Close  = final price when market closed
Volume = number of shares traded
```

This project currently uses close-to-close movement:

```cpp
dailyReturn = todayClose / yesterdayClose - 1.0;
```

The z-score is also based on closing prices:

```text
z-score = (close - movingAverageOfClose) / standardDeviationOfClose
```

## Are we missing intraday highs by using only close prices?

Yes.

Example:

```text
Open  = 100
High  = 120
Low   = 98
Close = 102
```

The price touched 120 during the day, but a close-only strategy sees only 102 at the end of the day. So it cannot claim it sold at 120 unless the strategy had a pre-planned intraday rule.

Close-only data is not invalid. It just means the strategy is an end-of-day strategy:

1. Wait for the market close.
2. Read the close price.
3. Calculate the signal.
4. Trade on the next day.

## Can we see the high live in real trading?

Yes. In live trading, we can see prices move in real time.

The limitation is in daily backtesting data. A daily candle tells us the final summary:

```text
Open, High, Low, Close
```

But it does not tell us the exact sequence inside the day:

```text
Did the high happen before or after our entry?
Was our order already placed?
Was there enough liquidity?
Did the price stay there long enough to fill?
```

Using daily `High` is valid only if the rule was known before the high happened.

Valid example:

```text
After buying, place a take-profit sell order at +5%.
If tomorrow's high reaches that level, assume the order was filled.
```

Invalid example:

```text
At the end of the day, look back at the highest price and assume we sold exactly there.
```

The second case is lookahead bias because it uses information from the future to improve the past trade.

## What are `lookback`, `zScores`, and `positions`?

These are the main strategy variables.

### `lookback`

`lookback = 20` means the strategy uses the last 20 trading days as its recent history.

For example, on day 21, it looks at days 1 to 20 and calculates:

```text
20-day average
20-day standard deviation
```

Then it compares today's close against that recent history.

So `lookback` is the memory window of the strategy.

### `zScores`

A z-score tells us how far today's price is from its recent average.

Formula:

```text
zScore = (todayClose - 20DayAverage) / 20DayStandardDeviation
```

Interpretation:

```text
zScore = 0   -> price is near the recent average
zScore = +1  -> price is 1 standard deviation above average
zScore = -1  -> price is 1 standard deviation below average
zScore = -2  -> price is very low compared to recent average
```

The strategy uses this idea:

```text
if zScore < -2:
    price may have fallen too much, so enter a long trade
```

This is based on mean reversion: the idea that an unusually low price may move back toward its average.

### `positions`

`positions` tells us whether the strategy wants to hold the stock on each day.

It is an array of `0` and `1`:

```text
position = 0 -> not holding
position = 1 -> holding / long
```

Example:

```text
Date    Close   zScore   Position
Day 1   100     -        0
Day 2   101     -        0
Day 21  90      -2.3     1   buy signal
Day 22  92      -1.5     1   still holding
Day 23  98       0.2     0   exit signal
```

In code:

```cpp
std::vector<double> zScores;
std::vector<int> positions;
```

means:

```text
zScores[i]   = how extreme the price is on day i
positions[i] = whether the strategy wants to hold on day i
```

The backtest uses yesterday's position for today's return:

```cpp
shiftedPosition = signals.positions[i - 1];
```

This avoids lookahead bias because a signal decided after today's close can only affect the next trading day.

## What are `transactionCost` and `tradingDaysPerYear`?

These are backtesting assumptions.

### `transactionCost`

`transactionCost = 0.001` means we assume every trade costs 0.1%.

```text
0.001 * 100 = 0.1%
```

Example:

```text
Trade value = 100000
Cost        = 100000 * 0.001 = 100
```

This represents brokerage, taxes, exchange charges, STT, small execution cost, and similar expenses. It is simplified, but better than pretending trading is free.

In the code, when the position changes, we subtract this cost from the strategy return:

```cpp
strategyReturn -= transactionCost;
```

### `tradingDaysPerYear`

`tradingDaysPerYear = 252` means we assume about 252 market trading days in one year.

A calendar year has 365 days, but markets are closed on weekends and holidays.

We use 252 to annualize metrics such as:

```text
annualized return
Sharpe ratio
```

For example, daily Sharpe is scaled to yearly Sharpe using:

```text
annualSharpe = dailySharpe * sqrt(252)
```

## What is Sharpe ratio?

Sharpe ratio measures return compared to risk/volatility.

Simple idea:

```text
Higher return is good.
Lower volatility is good.
Sharpe combines both.
```

Simplified formula:

```text
Sharpe ratio = average return / standard deviation of returns
```

For daily returns, it is annualized:

```text
Sharpe = dailyAverageReturn / dailyStdDev * sqrt(252)
```

Two strategies can have the same return but very different risk:

```text
Strategy A: smooth +10% per year
Strategy B: wild +10% per year with big crashes and jumps
```

Both made 10%, but Strategy A is usually better. Sharpe ratio rewards smoother returns.

Rough interpretation:

```text
Sharpe < 0  -> bad, losing after risk
Sharpe 0-1  -> weak/okay
Sharpe 1-2  -> good
Sharpe 2+   -> very strong
Sharpe 3+   -> suspiciously excellent, check overfitting/data issues
```

Professional Sharpe usually subtracts the risk-free rate:

```text
Sharpe = (strategy return - risk-free return) / volatility
```

The first version of this project ignores the risk-free rate to keep the calculation simple.

## Is standard deviation of returns volatility?

Yes. Standard deviation of returns is volatility.

More precisely:

```text
volatility = how much returns jump around from their average
```

Stable returns:

```text
+0.1%, +0.2%, +0.0%, +0.1%, +0.2%
```

Low standard deviation, so low volatility.

Wild returns:

```text
+5%, -4%, +6%, -7%, +3%
```

High standard deviation, so high volatility.

In finance, when people say a stock or strategy is volatile, they usually mean its returns have high standard deviation.

Sharpe ratio uses this idea:

```text
Sharpe = average return / volatility
```

So Sharpe asks:

```text
Are we getting enough return for the volatility we are taking?
```

Volatility is often quoted annually:

```text
annualVolatility = dailyStdDev * sqrt(252)
```

## Why export daily results?

The terminal metrics show the final summary, but they do not show what happened on each day.

The daily results CSV helps us inspect the strategy step by step:

```text
Date
Close
ZScore
Position
DailyReturn
EquityCurve
```

This is useful because a backtest should not only produce a final return. We should also be able to verify:

```text
When did the strategy enter?
When did it exit?
Was the signal shifted correctly?
How did the equity curve move?
Were the daily returns reasonable?
```

So the results file is a debugging and learning tool, not just an output file.

## Why export a trade log?

The daily results file shows every row, but sometimes we only want to see the actual trades.

The trade log shows:

```text
EntryDate
EntryPrice
ExitDate
ExitPrice
HoldingDays
GrossReturn
```

This makes it easier to answer:

```text
How many full trades happened?
Where did each trade enter and exit?
How much did each trade make or lose before costs?
Did the strategy hold too long or exit quickly?
```

The current trade prices are based on close-to-close daily data, so they are still an approximation. Later, if we add `Open`, `High`, `Low`, and `Close`, we can model execution more realistically.

## Where can we get real CSV data?

Manual Yahoo Finance downloads may not always show a clear download button.

For this project, a practical workflow is:

```text
Use Python only to download CSV data.
Use C++ to run the actual backtest.
```

The helper script uses Yahoo Finance ticker symbols such as:

```text
RELIANCE.NS
NIFTYBEES.NS
BANKBEES.NS
```

and writes:

```text
data/RELIANCE.csv
data/NIFTYBEES.csv
data/BANKBEES.csv
```

This keeps the C++ project focused on strategy logic while still making data collection easy.

On systems where global `pip install` is blocked, use a local virtual environment:

```bash
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
.venv/bin/python scripts/download_yahoo_data.py
```

## What is `Adj Close`?

`Adj Close` means adjusted close.

It is the closing price adjusted for corporate actions such as:

```text
stock splits
bonus issues
dividends
rights issues
```

This matters because raw `Close` can show fake crashes or jumps.

Example:

```text
Raw Close:
Day 1: 1000
Day 2: 100
Looks like: -90%
```

If that happened because of a split, investors did not actually lose 90%. They now own more shares. Adjusted close changes the old prices so return calculations stay realistic:

```text
Adjusted Close:
Day 1: 100
Day 2: 100
Looks like: 0%
```

For backtesting with daily historical data, `Adj Close` is often better than raw `Close` because it avoids fake returns from splits and dividends.

The CSV reader now uses this rule:

```text
If Adj Close exists, use Adj Close.
Otherwise, use Close.
```

## Why did NIFTYBEES and BANKBEES show a -90% drawdown?

The data quality check found one suspicious day in both ETF files:

```text
NIFTYBEES: 2019-12-19 had about a -89.9% daily move
BANKBEES:  2019-12-19 had about a -90.0% daily move
```

That is probably not a real ETF crash. It is likely a data artifact from Yahoo Finance, such as a split or adjustment issue.

This explains why buy-and-hold max drawdown looked close to `-90%`. A single bad price can make the benchmark look much worse and can distort the strategy result.

The next step is to either:

```text
use a more reliable ETF data source,
fix/remove clearly bad rows,
or document that those ETF results are not trustworthy yet.
```

## Why make parameters configurable?

At first, the strategy used fixed values:

```text
lookback = 20
entryZScore = -2
exitZScore = 0
transactionCost = 0.001
```

But strategy performance can change a lot when these values change.

For example, testing `lookback = 60` instead of `20` asks:

```text
Does a longer memory window produce better mean-reversion signals?
```

The program now supports:

```bash
./build/backtester <csv-file> <lookback> <entry-z> <exit-z> <transaction-cost>
```

Example:

```bash
./build/backtester data/RELIANCE.csv 60 -2 0 0.001
```

## What is the slope-confirmed entry/exit idea?

The original strategy enters when price is very low by z-score and exits when z-score returns above 0.

A different idea is:

```text
Entry:
zScore < -0.5
and recent price slope has turned positive

Exit:
after entry, exit when recent positive slope is strong enough
```

This avoids buying only because price is low. It waits for some evidence that price has started recovering.

In this project, slope is currently defined simply:

```text
slope = today's adjusted close - adjusted close N trading days ago
```

So if `slopeLookback = 7`:

```text
positive slope -> today's price is above price 7 trading days ago
```

Run it like:

```bash
./build/backtester data/RELIANCE.csv 60 -1.0 0 0.001 7 profit_stop
```

When `slopeLookback = 0`, the program uses the original z-score-only exit rule.

In slope mode:

```text
Entry requires:
zScore < entryZScore
slope > 0

Exit requires:
last 7 trading days gained at least 3%
or both 7-day blocks in the last 14 trading days gained at least 2%
or all three 7-day blocks in the last 21 trading days gained at least 1%
or, in `profit_stop` mode, the last 7 trading days lost at least 5%
```

This exits into strength instead of waiting for weakness. The idea is to take profit after a rebound has happened, even if the rebound was gradual:

```text
fast rebound: 3% in 7 trading days
steady rebound: 2% + 2% across two 7-day blocks
slow rebound: 1% + 1% + 1% across three 7-day blocks
```

The supported slope exit modes are:

```text
neg         -> exit when slope turns negative
profit      -> exit into strength using only the positive-slope profit rules
profit_stop -> profit rules plus a -5% 7-day negative-slope stop
```
