#pragma once

#include <string>
#include <vector>

struct PerformanceMetrics {
    std::string name;
    double totalReturn = 0.0;
    double annualizedReturn = 0.0;
    double sharpeRatio = 0.0;
    double maxDrawdown = 0.0;
    int trades = 0;
};

PerformanceMetrics calculateMetrics(
    const std::string& name,
    const std::vector<double>& dailyReturns,
    int trades,
    int tradingDaysPerYear
);

void printMetricsTable(
    const PerformanceMetrics& strategy,
    const PerformanceMetrics& buyAndHold
);
