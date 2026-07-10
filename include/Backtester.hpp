#pragma once

#include "PriceData.hpp"
#include "Strategy.hpp"

#include <string>
#include <vector>

struct BacktestConfig {
    double transactionCost = 0.001;
    int tradingDaysPerYear = 252;
};

struct Trade {
    std::string entryDate;
    double entryPrice = 0.0;
    std::string exitDate;
    double exitPrice = 0.0;
    int holdingDays = 0;
    double grossReturn = 0.0;
};

struct BacktestResult {
    std::vector<double> dailyReturns;
    std::vector<double> equityCurve;
    std::vector<Trade> tradeLog;
    int trades = 0;
};

BacktestResult runBacktest(
    const PriceSeries& prices,
    const StrategySignals& signals,
    const BacktestConfig& config
);

std::vector<double> calculateBuyAndHoldReturns(const PriceSeries& prices);
