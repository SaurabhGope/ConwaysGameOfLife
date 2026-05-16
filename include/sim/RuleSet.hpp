#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include <glm/vec3.hpp>
#include "core/AppConfig.hpp"

struct RuleSet
{
    std::array<bool, 27> birth{};
    std::array<bool, 27> survive{};
    BoundaryMode boundary = BoundaryMode::FixedDead;

    static RuleSet FromString(std::string_view spec, BoundaryMode boundaryMode);
    bool ShouldLive(bool currentlyAlive, uint8_t neighborCount) const;
};

struct ThresholdRange
{
    float min = 0.0f;
    float max = 0.0f;

    bool Contains(float value) const;
};

struct WeightedRuleSet
{
    std::array<float, 26> neighborWeights{};
    ThresholdRange birth{5.0f, 5.0f};
    ThresholdRange survive{4.0f, 6.0f};
    BoundaryMode boundary = BoundaryMode::FixedDead;

    static WeightedRuleSet FromRuleSet(const RuleSet& rules);
    static WeightedRuleSet FromConfig(const SimulationConfig& config, BoundaryMode boundaryMode);
    static const std::array<glm::ivec3, 26>& NeighborOffsets();

    bool ShouldLive(bool currentlyAlive, float weightedNeighborSum) const;
};
