#include "Backtester.hpp"
#include "CsvReader.hpp"
#include "Metrics.hpp"
#include "Strategy.hpp"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    const std::string filePath = argc >= 2 ? argv[1] : "data/sample_prices.csv";

    try {
        const PriceSeries prices = readPriceCsv(filePath);

        const StrategyConfig strategyConfig;
        const BacktestConfig backtestConfig;

        const StrategySignals signals = generateSignals(prices, strategyConfig);
        const BacktestResult result = runBacktest(prices, signals, backtestConfig);
        const std::vector<double> buyAndHoldReturns = calculateBuyAndHoldReturns(prices);

        const PerformanceMetrics strategyMetrics = calculateMetrics(
            "Z-Score",
            result.dailyReturns,
            result.trades,
            backtestConfig.tradingDaysPerYear
        );

        const PerformanceMetrics buyAndHoldMetrics = calculateMetrics(
            "Buy & Hold",
            buyAndHoldReturns,
            0,
            backtestConfig.tradingDaysPerYear
        );

        std::cout << "Loaded " << prices.size() << " rows from " << filePath << "\n";
        printMetricsTable(strategyMetrics, buyAndHoldMetrics);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
