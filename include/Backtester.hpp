#pragma once

#include "PriceData.hpp"
#include "Strategy.hpp"

#include <vector>

struct BacktestConfig {
    double transactionCost = 0.001;
    int tradingDaysPerYear = 252;
};

struct BacktestResult {
    std::vector<double> dailyReturns;
    std::vector<double> equityCurve;
    int trades = 0;
};

BacktestResult runBacktest(
    const PriceSeries& prices,
    const StrategySignals& signals,
    const BacktestConfig& config
);

std::vector<double> calculateBuyAndHoldReturns(const PriceSeries& prices);
