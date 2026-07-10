#pragma once

#include "PriceData.hpp"

#include <string>

PriceSeries readPriceCsv(const std::string& filePath);
