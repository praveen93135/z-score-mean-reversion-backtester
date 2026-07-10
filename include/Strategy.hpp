#pragma once

#include "PriceData.hpp"

#include <vector>

enum class SlopeExitMode {
    NegativeSlope,
    PositiveProfit
};

struct StrategyConfig {
    int lookback = 20;
    double entryZScore = -2.0;
    double exitZScore = 0.0;
    int slopeLookback = 0;
    SlopeExitMode slopeExitMode = SlopeExitMode::PositiveProfit;
};

struct StrategySignals {
    std::vector<double> zScores;
    std::vector<int> positions;
};

StrategySignals generateSignals(const PriceSeries& prices, const StrategyConfig& config);
