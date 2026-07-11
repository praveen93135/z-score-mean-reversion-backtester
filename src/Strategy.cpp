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

double priceSlope(const PriceSeries& prices, int index, int slopeLookback) {
    if (slopeLookback <= 0 || index < slopeLookback) {
        return 0.0;
    }

    return prices[index].close - prices[index - slopeLookback].close;
}

double periodReturn(const PriceSeries& prices, int startIndex, int endIndex) {
    if (startIndex < 0 || endIndex <= startIndex || prices[startIndex].close == 0.0) {
        return 0.0;
    }

    return (prices[endIndex].close / prices[startIndex].close) - 1.0;
}

bool hasPositiveSlopeExit(const PriceSeries& prices, int index, int slopeLookback) {
    if (slopeLookback <= 0 || index < slopeLookback * 3) {
        return false;
    }

    const double lastBlock = periodReturn(prices, index - slopeLookback, index);
    const double middleBlock = periodReturn(prices, index - slopeLookback * 2, index - slopeLookback);
    const double firstBlock = periodReturn(prices, index - slopeLookback * 3, index - slopeLookback * 2);

    const bool oneStrongBlock = lastBlock >= 0.03;
    const bool twoGoodBlocks = middleBlock >= 0.02 && lastBlock >= 0.02;
    const bool threeSmallBlocks = firstBlock >= 0.01 && middleBlock >= 0.01 && lastBlock >= 0.01;

    return oneStrongBlock || twoGoodBlocks || threeSmallBlocks;
}

bool hasSlowProfitExit(const PriceSeries& prices, int index, int slopeLookback, double entryPrice) {
    const int slowLookback = slopeLookback * 2;
    if (entryPrice <= 0.0 || slowLookback <= 0 || index < slowLookback) {
        return false;
    }

    const bool tradeIsProfitable = prices[index].close > entryPrice;
    const bool slowSlopeTurnedNegative = priceSlope(prices, index, slowLookback) < 0.0;
    return tradeIsProfitable && slowSlopeTurnedNegative;
}

bool hasProtectiveExit(const PriceSeries& prices, int index, double entryPrice) {
    if (entryPrice <= 0.0) {
        return false;
    }

    const double tradeReturn = (prices[index].close / entryPrice) - 1.0;
    return tradeReturn <= -0.25;
}

bool passesTrendFilter(const PriceSeries& prices, int index, int trendLookback) {
    if (trendLookback <= 0) {
        return true;
    }
    if (index < trendLookback) {
        return false;
    }

    const double trendMean = rollingMean(prices, index - trendLookback, index);
    return prices[index].close > trendMean * 0.60;
}

bool shouldExitSlopeMode(
    const PriceSeries& prices,
    int index,
    int slopeLookback,
    SlopeExitMode mode,
    double entryPrice
) {
    const double slope = priceSlope(prices, index, slopeLookback);

    if (mode == SlopeExitMode::NegativeSlope) {
        return slope < 0.0;
    }
    if (mode == SlopeExitMode::SlowProfitWithStop) {
        return hasSlowProfitExit(prices, index, slopeLookback, entryPrice)
            || hasProtectiveExit(prices, index, entryPrice);
    }

    return hasPositiveSlopeExit(prices, index, slopeLookback);
}

} // namespace

StrategySignals generateSignals(const PriceSeries& prices, const StrategyConfig& config) {
    const int n = static_cast<int>(prices.size());
    StrategySignals signals;
    signals.zScores.assign(n, 0.0);
    signals.positions.assign(n, 0);

    bool inPosition = false;
    double entryPrice = 0.0;

    for (int i = config.lookback; i < n; ++i) {
        const int start = i - config.lookback;
        const double mean = rollingMean(prices, start, i);
        const double stdDev = rollingStdDev(prices, start, i, mean);

        if (stdDev == 0.0) {
            signals.zScores[i] = 0.0;
        } else {
            signals.zScores[i] = (prices[i].close - mean) / stdDev;
        }

        if (config.slopeLookback > 0) {
            const double slope = priceSlope(prices, i, config.slopeLookback);
            const bool trendAllowsEntry = passesTrendFilter(prices, i, config.trendLookback);

            if (!inPosition && signals.zScores[i] < config.entryZScore && slope > 0.0 && trendAllowsEntry) {
                inPosition = true;
                entryPrice = prices[i].close;
            } else if (
                inPosition
                && shouldExitSlopeMode(prices, i, config.slopeLookback, config.slopeExitMode, entryPrice)
            ) {
                inPosition = false;
                entryPrice = 0.0;
            }
        } else {
            if (!inPosition && signals.zScores[i] < config.entryZScore) {
                inPosition = true;
                entryPrice = prices[i].close;
            } else if (inPosition && signals.zScores[i] > config.exitZScore) {
                inPosition = false;
                entryPrice = 0.0;
            }
        }

        signals.positions[i] = inPosition ? 1 : 0;
    }

    return signals;
}
