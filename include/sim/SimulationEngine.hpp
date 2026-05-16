#pragma once

#include <cstdint>
#include <vector>
#include "sim/Grid3D.hpp"
#include "sim/RuleSet.hpp"

struct SimulationStats
{
    uint64_t generation = 0;
    uint64_t aliveCells = 0;
    double lastStepMs = 0.0;
};

class SimulationEngine
{
public:
    SimulationEngine(Grid3D initialGrid, RuleSet rules);
    SimulationEngine(Grid3D initialGrid, WeightedRuleSet rules);

    void Step(uint32_t iterations = 1);
    void Reset(Grid3D newInitialGrid);
    void SetRules(WeightedRuleSet rules);

    const Grid3D& Current() const;
    const Grid3D& Next() const;
    const SimulationStats& Stats() const;

    std::vector<uint32_t> BuildAliveLinearIndexList() const;

private:
    uint8_t CountNeighbors(const Grid3D& grid, uint32_t x, uint32_t y, uint32_t z) const;
    float WeightedNeighborSum(const Grid3D& grid, uint32_t x, uint32_t y, uint32_t z) const;

    Grid3D m_current;
    Grid3D m_next;
    WeightedRuleSet m_rules;
    SimulationStats m_stats;
};
