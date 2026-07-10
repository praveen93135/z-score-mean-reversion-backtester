#include "Backtester.hpp"

std::vector<double> calculateBuyAndHoldReturns(const PriceSeries& prices) {
    std::vector<double> returns(prices.size(), 0.0);

    for (int i = 1; i < static_cast<int>(prices.size()); ++i) {
        returns[i] = (prices[i].close / prices[i - 1].close) - 1.0;
    }

    return returns;
}

BacktestResult runBacktest(
    const PriceSeries& prices,
    const StrategySignals& signals,
    const BacktestConfig& config
) {
    const int n = static_cast<int>(prices.size());
    BacktestResult result;
    result.dailyReturns.assign(n, 0.0);
    result.equityCurve.assign(n, 1.0);

    int previousPosition = 0;
    Trade openTrade;
    bool hasOpenTrade = false;
    int openTradeIndex = 0;

    for (int i = 1; i < n; ++i) {
        const int shiftedPosition = signals.positions[i - 1];
        const double assetReturn = (prices[i].close / prices[i - 1].close) - 1.0;
        double strategyReturn = shiftedPosition * assetReturn;

        if (shiftedPosition != previousPosition) {
            strategyReturn -= config.transactionCost;
            ++result.trades;

            if (shiftedPosition == 1) {
                openTrade = Trade{};
                openTrade.entryDate = prices[i - 1].date;
                openTrade.entryPrice = prices[i - 1].close;
                hasOpenTrade = true;
                openTradeIndex = i - 1;
            } else if (hasOpenTrade) {
                openTrade.exitDate = prices[i - 1].date;
                openTrade.exitPrice = prices[i - 1].close;
                openTrade.holdingDays = (i - 1) - openTradeIndex;
                openTrade.grossReturn = (openTrade.exitPrice / openTrade.entryPrice) - 1.0;
                result.tradeLog.push_back(openTrade);
                hasOpenTrade = false;
            }
        }

        result.dailyReturns[i] = strategyReturn;
        result.equityCurve[i] = result.equityCurve[i - 1] * (1.0 + strategyReturn);
        previousPosition = shiftedPosition;
    }

    if (hasOpenTrade) {
        openTrade.exitDate = prices.back().date;
        openTrade.exitPrice = prices.back().close;
        openTrade.holdingDays = (n - 1) - openTradeIndex;
        openTrade.grossReturn = (openTrade.exitPrice / openTrade.entryPrice) - 1.0;
        result.tradeLog.push_back(openTrade);
    }

    return result;
}
