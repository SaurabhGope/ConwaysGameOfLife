#include "sim/SimulationEngine.hpp"
#include <cstddef>

#include <chrono>
#include <utility>

SimulationEngine::SimulationEngine(Grid3D initialGrid, RuleSet rules)
    : SimulationEngine(std::move(initialGrid), WeightedRuleSet::FromRuleSet(rules))
{
}

SimulationEngine::SimulationEngine(Grid3D initialGrid, WeightedRuleSet rules)
    : m_current(std::move(initialGrid))
    , m_next(m_current.Dims())
    , m_rules(rules)
{
    m_stats.aliveCells = m_current.AliveCount();
}

void SimulationEngine::Step(uint32_t iterations)
{
    using clock = std::chrono::high_resolution_clock;

    const auto start = clock::now();

    for (uint32_t iteration = 0; iteration < iterations; ++iteration)
    {
        const auto dims = m_current.Dims();
        m_next.Clear();

        for (uint32_t z = 0; z < dims.z; ++z)
        {
            for (uint32_t y = 0; y < dims.y; ++y)
            {
                for (uint32_t x = 0; x < dims.x; ++x)
                {
                    const bool alive = m_current.Get(x, y, z);
                    const float neighbors = WeightedNeighborSum(m_current, x, y, z);
                    const bool nextAlive = m_rules.ShouldLive(alive, neighbors);
                    m_next.Set(x, y, z, nextAlive);
                }
            }
        }

        std::swap(m_current, m_next);
        ++m_stats.generation;
    }

    const auto end = clock::now();
    m_stats.aliveCells = m_current.AliveCount();
    m_stats.lastStepMs = std::chrono::duration<double, std::milli>(end - start).count();
}

void SimulationEngine::Reset(Grid3D newInitialGrid)
{
    m_current = std::move(newInitialGrid);
    m_next = Grid3D(m_current.Dims());
    m_stats = {};
    m_stats.aliveCells = m_current.AliveCount();
}

void SimulationEngine::SetRules(WeightedRuleSet rules)
{
    m_rules = rules;
}

const Grid3D& SimulationEngine::Current() const
{
    return m_current;
}

const Grid3D& SimulationEngine::Next() const
{
    return m_next;
}

const SimulationStats& SimulationEngine::Stats() const
{
    return m_stats;
}

std::vector<uint32_t> SimulationEngine::BuildAliveLinearIndexList() const
{
    std::vector<uint32_t> aliveIndices;
    aliveIndices.reserve(static_cast<size_t>(m_current.AliveCount()));

    const auto dims = m_current.Dims();
    for (uint32_t z = 0; z < dims.z; ++z)
    {
        for (uint32_t y = 0; y < dims.y; ++y)
        {
            for (uint32_t x = 0; x < dims.x; ++x)
            {
                if (m_current.Get(x, y, z))
                {
                    aliveIndices.push_back(static_cast<uint32_t>(m_current.LinearIndex(x, y, z)));
                }
            }
        }
    }

    return aliveIndices;
}

uint8_t SimulationEngine::CountNeighbors(const Grid3D& grid, uint32_t x, uint32_t y, uint32_t z) const
{
    const auto dims = grid.Dims();
    uint8_t count = 0;

    for (int32_t dz = -1; dz <= 1; ++dz)
    {
        for (int32_t dy = -1; dy <= 1; ++dy)
        {
            for (int32_t dx = -1; dx <= 1; ++dx)
            {
                if (dx == 0 && dy == 0 && dz == 0)
                {
                    continue;
                }

                const int64_t nx = static_cast<int64_t>(x) + dx;
                const int64_t ny = static_cast<int64_t>(y) + dy;
                const int64_t nz = static_cast<int64_t>(z) + dz;

                if (m_rules.boundary == BoundaryMode::FixedDead)
                {
                    if (nx < 0 || ny < 0 || nz < 0)
                    {
                        continue;
                    }

                    if (nx >= static_cast<int64_t>(dims.x) ||
                        ny >= static_cast<int64_t>(dims.y) ||
                        nz >= static_cast<int64_t>(dims.z))
                    {
                        continue;
                    }
                }
                else
                {
                    const auto wrap = [](int64_t value, uint32_t size) {
                        const int64_t span = static_cast<int64_t>(size);
                        return static_cast<uint32_t>((value % span + span) % span);
                    };

                    if (grid.Get(wrap(nx, dims.x), wrap(ny, dims.y), wrap(nz, dims.z)))
                    {
                        ++count;
                    }
                    continue;
                }

                if (grid.Get(static_cast<uint32_t>(nx), static_cast<uint32_t>(ny), static_cast<uint32_t>(nz)))
                {
                    ++count;
                }
            }
        }
    }

    return count;
}

float SimulationEngine::WeightedNeighborSum(const Grid3D& grid, uint32_t x, uint32_t y, uint32_t z) const
{
    const auto dims = grid.Dims();
    float sum = 0.0f;
    const auto& offsets = WeightedRuleSet::NeighborOffsets();

    for (size_t i = 0; i < offsets.size(); ++i)
    {
        const auto& offset = offsets[i];
        const int64_t nx = static_cast<int64_t>(x) + offset.x;
        const int64_t ny = static_cast<int64_t>(y) + offset.y;
        const int64_t nz = static_cast<int64_t>(z) + offset.z;

        if (m_rules.boundary == BoundaryMode::FixedDead)
        {
            if (nx < 0 || ny < 0 || nz < 0 ||
                nx >= static_cast<int64_t>(dims.x) ||
                ny >= static_cast<int64_t>(dims.y) ||
                nz >= static_cast<int64_t>(dims.z))
            {
                continue;
            }

            if (grid.Get(static_cast<uint32_t>(nx), static_cast<uint32_t>(ny), static_cast<uint32_t>(nz)))
            {
                sum += m_rules.neighborWeights[i];
            }
            continue;
        }

        const auto wrap = [](int64_t value, uint32_t size) {
            const int64_t span = static_cast<int64_t>(size);
            return static_cast<uint32_t>((value % span + span) % span);
        };

        if (grid.Get(wrap(nx, dims.x), wrap(ny, dims.y), wrap(nz, dims.z)))
        {
            sum += m_rules.neighborWeights[i];
        }
    }

    return sum;
}
