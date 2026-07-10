#include "Strategy.hpp"

#include <cmath>

namespace {

double rollingMean(const PriceSeries& prices, int start, int endExclusive) {
    double sum = 0.0;
    for (int i = start; i < endExclusive; ++i) {
        sum += prices[i].close;
    }
    return sum / static_cast<double>(endExclusive - start);
}

double rollingStdDev(const PriceSeries& prices, int start, int endExclusive, double mean) {
    double squaredDiffs = 0.0;
    for (int i = start; i < endExclusive; ++i) {
        const double diff = prices[i].close - mean;
        squaredDiffs += diff * diff;
    }

    const double variance = squaredDiffs / static_cast<double>(endExclusive - start);
    return std::sqrt(variance);
}

} // namespace

StrategySignals generateSignals(const PriceSeries& prices, const StrategyConfig& config) {
    const int n = static_cast<int>(prices.size());
    StrategySignals signals;
    signals.zScores.assign(n, 0.0);
    signals.positions.assign(n, 0);

    bool inPosition = false;

    for (int i = config.lookback; i < n; ++i) {
        const int start = i - config.lookback;
        const double mean = rollingMean(prices, start, i);
        const double stdDev = rollingStdDev(prices, start, i, mean);

        if (stdDev == 0.0) {
            signals.zScores[i] = 0.0;
        } else {
            signals.zScores[i] = (prices[i].close - mean) / stdDev;
        }

        if (!inPosition && signals.zScores[i] < config.entryZScore) {
            inPosition = true;
        } else if (inPosition && signals.zScores[i] > config.exitZScore) {
            inPosition = false;
        }

        signals.positions[i] = inPosition ? 1 : 0;
    }

    return signals;
}
