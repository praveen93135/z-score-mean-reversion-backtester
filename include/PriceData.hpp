#pragma once

#include <string>
#include <vector>

struct PricePoint {
    std::string date;
    double close = 0.0;
};

using PriceSeries = std::vector<PricePoint>;
