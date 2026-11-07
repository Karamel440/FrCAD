#pragma once

#include <cstddef>
#include <limits>
#include <optional>
#include <vector>

namespace well_assignment {

inline constexpr double searchRadius = 4.0;

struct Candidate {
    std::size_t label;
    double distance;
};

struct Assignment {
    std::optional<std::size_t> label;
    bool ambiguous = false;
};

namespace detail {

using Indices = std::vector<std::size_t>;
using CostMatrix = std::vector<std::vector<double>>;
inline constexpr double forbidden = std::numeric_limits<double>::infinity();
inline constexpr double ambiguityMargin = 0.15;

struct Minimum {
    Indices columnForRow;
    double cost = 0.0;
};

// Rectangular Hungarian algorithm. Each row must choose a distinct column.
inline Minimum minimumAssignment(const CostMatrix& cost) {
    const auto rows = cost.size();
    const auto columns = cost.front().size();
    // Slot zero is the starting column of the current augmenting path.
    std::vector<double> rowPotential(rows + 1), columnPotential(columns + 1);
    Indices owner(columns + 1), previous(columns + 1);

    for (std::size_t row = 1; row <= rows; ++row) {
        owner[0] = row;
        std::size_t column = 0;
        std::vector<double> slack(columns + 1, forbidden);
        std::vector<bool> visited(columns + 1);
        do {
            visited[column] = true;
            const auto currentRow = owner[column];
            double step = forbidden;
            std::size_t next = 0;
            for (std::size_t j = 1; j <= columns; ++j) {
                if (visited[j]) continue;
                const double reduced = cost[currentRow - 1][j - 1] -
                                       rowPotential[currentRow] -
                                       columnPotential[j];
                if (reduced < slack[j]) {
                    slack[j] = reduced;
                    previous[j] = column;
                }
                if (slack[j] < step) {
                    step = slack[j];
                    next = j;
                }
            }
            if (next == 0) return {{}, forbidden};
            for (std::size_t j = 0; j <= columns; ++j) {
                if (visited[j]) {
                    rowPotential[owner[j]] += step;
                    columnPotential[j] -= step;
                } else {
                    slack[j] -= step;
                }
            }
            column = next;
        } while (owner[column] != 0);

        do {
            const auto predecessor = previous[column];
            owner[column] = owner[predecessor];
            column = predecessor;
        } while (column != 0);
    }

    Minimum result{Indices(rows)};
    for (std::size_t col = 1; col <= columns; ++col) {
        if (owner[col] != 0) {
            const auto row = owner[col] - 1;
            result.columnForRow[row] = col - 1;
            result.cost += cost[row][col - 1];
        }
    }
    return result;
}

inline void assignComponent(
    const Indices& wells, const Indices& labels, const Indices& localLabel,
    const std::vector<std::vector<Candidate>>& candidates,
    std::vector<Assignment>& result) {
    CostMatrix cost(wells.size(), std::vector<double>(
                                      labels.size() + wells.size(), forbidden));
    for (std::size_t row = 0; row < wells.size(); ++row) {
        for (const auto& candidate : candidates[wells[row]])
            cost[row][localLabel[candidate.label]] = candidate.distance;
        // Each well has its own "without a number" column.
        cost[row][labels.size() + row] = searchRadius;
    }
    const auto best = minimumAssignment(cost);
    for (std::size_t row = 0; row < wells.size(); ++row) {
        const auto column = best.columnForRow[row];
        const auto original = cost[row][column];
        cost[row][column] = forbidden;
        const double alternative = minimumAssignment(cost).cost;
        cost[row][column] = original;

        auto& match = result[wells[row]];
        if (alternative <= best.cost + ambiguityMargin + 1e-9)
            match.ambiguous = true;
        else if (column < labels.size())
            match.label = labels[column];
    }
}

}  // namespace detail

inline std::vector<Assignment> solve(
    const std::vector<std::vector<Candidate>>& candidates,
    std::size_t labelCount) {
    std::vector<Assignment> result(candidates.size());
    std::vector<detail::Indices> wellsForLabel(labelCount);
    for (std::size_t well = 0; well < candidates.size(); ++well)
        for (const auto& candidate : candidates[well])
            wellsForLabel[candidate.label].push_back(well);

    std::vector<bool> seenWell(candidates.size()), seenLabel(labelCount);
    detail::Indices localLabel(labelCount);
    for (std::size_t start = 0; start < candidates.size(); ++start) {
        if (seenWell[start] || candidates[start].empty()) continue;
        detail::Indices wells{start}, labels;
        seenWell[start] = true;
        // Grow just the group of wells that compete for the same labels.
        for (std::size_t i = 0; i < wells.size(); ++i) {
            for (const auto& candidate : candidates[wells[i]]) {
                const auto label = candidate.label;
                if (seenLabel[label]) continue;
                seenLabel[label] = true;
                localLabel[label] = labels.size();
                labels.push_back(label);
                for (const auto neighbor : wellsForLabel[label]) {
                    if (!seenWell[neighbor]) {
                        seenWell[neighbor] = true;
                        wells.push_back(neighbor);
                    }
                }
            }
        }
        detail::assignComponent(wells, labels, localLabel, candidates, result);
    }
    return result;
}

}  // namespace well_assignment
