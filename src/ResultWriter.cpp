#include "ResultWriter.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace {

void createParentDirectory(const std::string& filePath) {
    const std::filesystem::path path(filePath);
    const std::filesystem::path parent = path.parent_path();

    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }
}

} // namespace

void writeDailyResultsCsv(
    const std::string& filePath,
    const PriceSeries& prices,
    const StrategySignals& signals,
    const BacktestResult& result
) {
    createParentDirectory(filePath);

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

void writeTradesCsv(
    const std::string& filePath,
    const BacktestResult& result
) {
    createParentDirectory(filePath);

    std::ofstream output(filePath);
    if (!output) {
        throw std::runtime_error("Could not write trades CSV: " + filePath);
    }

    output << "EntryDate,EntryPrice,ExitDate,ExitPrice,HoldingDays,GrossReturn\n";
    output << std::fixed << std::setprecision(6);

    for (const Trade& trade : result.tradeLog) {
        output << trade.entryDate << ","
               << trade.entryPrice << ","
               << trade.exitDate << ","
               << trade.exitPrice << ","
               << trade.holdingDays << ","
               << trade.grossReturn << "\n";
    }
}
