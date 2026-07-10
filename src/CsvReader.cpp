#include "CsvReader.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> cells;
    std::stringstream ss(line);
    std::string cell;

    while (std::getline(ss, cell, ',')) {
        cells.push_back(cell);
    }

    return cells;
}

int findColumn(const std::vector<std::string>& header, const std::string& columnName) {
    for (int i = 0; i < static_cast<int>(header.size()); ++i) {
        if (header[i] == columnName) {
            return i;
        }
    }
    return -1;
}

} // namespace

PriceSeries readPriceCsv(const std::string& filePath) {
    std::ifstream input(filePath);
    if (!input) {
        throw std::runtime_error("Could not open CSV file: " + filePath);
    }

    std::string line;
    if (!std::getline(input, line)) {
        throw std::runtime_error("CSV file is empty: " + filePath);
    }

    const std::vector<std::string> header = splitCsvLine(line);
    const int dateColumn = findColumn(header, "Date");
    const int adjustedCloseColumn = findColumn(header, "Adj Close");
    const int rawCloseColumn = findColumn(header, "Close");
    const int priceColumn = adjustedCloseColumn != -1 ? adjustedCloseColumn : rawCloseColumn;

    if (dateColumn == -1 || priceColumn == -1) {
        throw std::runtime_error("CSV must contain Date and either Adj Close or Close columns.");
    }

    PriceSeries prices;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> cells = splitCsvLine(line);
        const int neededColumns = std::max(dateColumn, priceColumn) + 1;
        if (static_cast<int>(cells.size()) < neededColumns) {
            continue;
        }

        PricePoint point;
        point.date = cells[dateColumn];
        if (cells[priceColumn].empty()) {
            continue;
        }
        point.close = std::stod(cells[priceColumn]);
        prices.push_back(point);
    }

    if (prices.size() < 2) {
        throw std::runtime_error("CSV must contain at least two price rows.");
    }

    std::sort(prices.begin(), prices.end(), [](const PricePoint& left, const PricePoint& right) {
        return left.date < right.date;
    });

    return prices;
}
