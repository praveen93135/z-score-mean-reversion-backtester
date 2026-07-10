#pragma once

#include "PriceData.hpp"

#include <vector>

struct StrategyConfig {
    int lookback = 20;
    double entryZScore = -2.0;
    double exitZScore = 0.0;
    int slopeLookback = 0;
};

struct StrategySignals {
    std::vector<double> zScores;
    std::vector<int> positions;
};

StrategySignals generateSignals(const PriceSeries& prices, const StrategyConfig& config);
