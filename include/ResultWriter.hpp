#pragma once

#include "Backtester.hpp"
#include "PriceData.hpp"
#include "Strategy.hpp"

#include <string>

void writeDailyResultsCsv(
    const std::string& filePath,
    const PriceSeries& prices,
    const StrategySignals& signals,
    const BacktestResult& result
);
