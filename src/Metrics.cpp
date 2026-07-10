#include "Metrics.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

namespace {

double calculateTotalReturn(const std::vector<double>& dailyReturns) {
    double equity = 1.0;
    for (double dailyReturn : dailyReturns) {
        equity *= (1.0 + dailyReturn);
    }
    return equity - 1.0;
}

double calculateAnnualizedReturn(double totalReturn, int days, int tradingDaysPerYear) {
    if (days <= 1) {
        return 0.0;
    }

    const double years = static_cast<double>(days) / static_cast<double>(tradingDaysPerYear);
    return std::pow(1.0 + totalReturn, 1.0 / years) - 1.0;
}

double calculateSharpeRatio(const std::vector<double>& dailyReturns, int tradingDaysPerYear) {
    if (dailyReturns.size() <= 1) {
        return 0.0;
    }

    const double mean = std::accumulate(dailyReturns.begin(), dailyReturns.end(), 0.0)
        / static_cast<double>(dailyReturns.size());

    double squaredDiffs = 0.0;
    for (double dailyReturn : dailyReturns) {
        const double diff = dailyReturn - mean;
        squaredDiffs += diff * diff;
    }

    const double variance = squaredDiffs / static_cast<double>(dailyReturns.size());
    const double stdDev = std::sqrt(variance);

    if (stdDev == 0.0) {
        return 0.0;
    }

    return (mean / stdDev) * std::sqrt(static_cast<double>(tradingDaysPerYear));
}

double calculateMaxDrawdown(const std::vector<double>& dailyReturns) {
    double equity = 1.0;
    double peak = 1.0;
    double maxDrawdown = 0.0;

    for (double dailyReturn : dailyReturns) {
        equity *= (1.0 + dailyReturn);
        if (equity > peak) {
            peak = equity;
        }

        const double drawdown = (equity / peak) - 1.0;
        if (drawdown < maxDrawdown) {
            maxDrawdown = drawdown;
        }
    }

    return maxDrawdown;
}

std::string formatPercent(double value) {
    std::ostringstream output;
    output << std::fixed << std::setprecision(2) << value * 100.0 << "%";
    return output.str();
}

} // namespace

PerformanceMetrics calculateMetrics(
    const std::string& name,
    const std::vector<double>& dailyReturns,
    int trades,
    int tradingDaysPerYear
) {
    PerformanceMetrics metrics;
    metrics.name = name;
    metrics.totalReturn = calculateTotalReturn(dailyReturns);
    metrics.annualizedReturn = calculateAnnualizedReturn(
        metrics.totalReturn,
        static_cast<int>(dailyReturns.size()),
        tradingDaysPerYear
    );
    metrics.sharpeRatio = calculateSharpeRatio(dailyReturns, tradingDaysPerYear);
    metrics.maxDrawdown = calculateMaxDrawdown(dailyReturns);
    metrics.trades = trades;
    return metrics;
}

void printMetricsTable(
    const PerformanceMetrics& strategy,
    const PerformanceMetrics& buyAndHold
) {
    std::cout << "\n";
    std::cout << std::left << std::setw(18) << "Metric"
              << std::right << std::setw(18) << strategy.name
              << std::setw(18) << buyAndHold.name << "\n";
    std::cout << std::string(54, '-') << "\n";

    std::cout << std::left << std::setw(18) << "Total Return" << std::right
              << std::setw(18) << formatPercent(strategy.totalReturn)
              << std::setw(18) << formatPercent(buyAndHold.totalReturn) << "\n";

    std::cout << std::left << std::setw(18) << "Annualized" << std::right
              << std::setw(18) << formatPercent(strategy.annualizedReturn)
              << std::setw(18) << formatPercent(buyAndHold.annualizedReturn) << "\n";

    std::cout << std::left << std::setw(18) << "Sharpe" << std::right
              << std::setw(17) << std::fixed << std::setprecision(2) << strategy.sharpeRatio
              << std::setw(18) << buyAndHold.sharpeRatio << "\n";

    std::cout << std::left << std::setw(18) << "Max Drawdown" << std::right
              << std::setw(18) << formatPercent(strategy.maxDrawdown)
              << std::setw(18) << formatPercent(buyAndHold.maxDrawdown) << "\n";

    std::cout << std::left << std::setw(18) << "Trades" << std::right
              << std::setw(17) << strategy.trades
              << std::setw(18) << buyAndHold.trades << "\n\n";
}
