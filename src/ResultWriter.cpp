#include "ResultWriter.hpp"

#include <fstream>
#include <iomanip>
#include <stdexcept>

void writeDailyResultsCsv(
    const std::string& filePath,
    const PriceSeries& prices,
    const StrategySignals& signals,
    const BacktestResult& result
) {
    std::ofstream output(filePath);
    if (!output) {
        throw std::runtime_error("Could not write results CSV: " + filePath);
    }

    output << "Date,Close,ZScore,Position,DailyReturn,EquityCurve\n";
    output << std::fixed << std::setprecision(6);

    for (int i = 0; i < static_cast<int>(prices.size()); ++i) {
        output << prices[i].date << ","
               << prices[i].close << ","
               << signals.zScores[i] << ","
               << signals.positions[i] << ","
               << result.dailyReturns[i] << ","
               << result.equityCurve[i] << "\n";
    }
}
