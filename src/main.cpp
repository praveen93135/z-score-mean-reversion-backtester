#include "Backtester.hpp"
#include "CsvReader.hpp"
#include "Metrics.hpp"
#include "ResultWriter.hpp"
#include "Strategy.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

SlopeExitMode parseSlopeExitMode(const std::string& mode) {
    if (mode == "neg") {
        return SlopeExitMode::NegativeSlope;
    }
    if (mode == "profit") {
        return SlopeExitMode::PositiveProfit;
    }
    if (mode == "profit_stop") {
        return SlopeExitMode::PositiveProfitWithStop;
    }

    throw std::runtime_error("Slope exit mode must be one of: neg, profit, profit_stop.");
}

std::string slopeExitModeName(SlopeExitMode mode) {
    if (mode == SlopeExitMode::NegativeSlope) {
        return "neg";
    }
    if (mode == SlopeExitMode::PositiveProfit) {
        return "profit";
    }
    return "profit_stop";
}

} // namespace

int main(int argc, char* argv[]) {
    const std::string filePath = argc >= 2 ? argv[1] : "data/sample_prices.csv";

    try {
        const PriceSeries prices = readPriceCsv(filePath);

        StrategyConfig strategyConfig;
        BacktestConfig backtestConfig;

        if (argc >= 3) {
            strategyConfig.lookback = std::stoi(argv[2]);
        }
        if (argc >= 4) {
            strategyConfig.entryZScore = std::stod(argv[3]);
        }
        if (argc >= 5) {
            strategyConfig.exitZScore = std::stod(argv[4]);
        }
        if (argc >= 6) {
            backtestConfig.transactionCost = std::stod(argv[5]);
        }
        if (argc >= 7) {
            strategyConfig.slopeLookback = std::stoi(argv[6]);
        }
        if (argc >= 8) {
            strategyConfig.slopeExitMode = parseSlopeExitMode(argv[7]);
        }

        if (strategyConfig.lookback <= 1) {
            throw std::runtime_error("Lookback must be greater than 1.");
        }
        if (strategyConfig.slopeLookback < 0) {
            throw std::runtime_error("Slope lookback cannot be negative.");
        }

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
        std::cout << "Config: lookback=" << strategyConfig.lookback
                  << ", entryZScore=" << strategyConfig.entryZScore
                  << ", exitZScore=" << strategyConfig.exitZScore
                  << ", transactionCost=" << backtestConfig.transactionCost
                  << ", slopeLookback=" << strategyConfig.slopeLookback
                  << ", slopeExitMode=" << slopeExitModeName(strategyConfig.slopeExitMode) << "\n";
        printMetricsTable(strategyMetrics, buyAndHoldMetrics);

        const std::string resultPath = "results/daily_results.csv";
        writeDailyResultsCsv(resultPath, prices, signals, result);
        std::cout << "Daily results written to " << resultPath << "\n";

        const std::string tradesPath = "results/trades.csv";
        writeTradesCsv(tradesPath, result);
        std::cout << "Trade log written to " << tradesPath << "\n";
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
