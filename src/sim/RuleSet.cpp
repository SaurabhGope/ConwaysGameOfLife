#include "sim/RuleSet.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <limits>
#include <stdexcept>

namespace
{
void CommitNeighborToken(RuleSet& rules, bool parsingBirth, bool parsingSurvival, int value)
{
    if (value < 0)
    {
        return;
    }

    if (value > 26)
    {
        throw std::invalid_argument("Neighbor count must be in range [0, 26].");
    }

    if (parsingBirth)
    {
        rules.birth[static_cast<size_t>(value)] = true;
        return;
    }

    if (parsingSurvival)
    {
        rules.survive[static_cast<size_t>(value)] = true;
        return;
    }

    throw std::invalid_argument("Rule string must start with B and include S sections.");
}
}

RuleSet RuleSet::FromString(std::string_view spec, BoundaryMode boundaryMode)
{
    if (spec.empty())
    {
        throw std::invalid_argument("Rule string cannot be empty.");
    }

    RuleSet rules{};
    rules.boundary = boundaryMode;

    bool parsingBirth = false;
    bool parsingSurvival = false;
    int token = -1;
    bool tokenHasSeparator = false;

    for (const char ch : spec)
    {
        if (ch == 'B' || ch == 'b')
        {
            CommitNeighborToken(rules, parsingBirth, parsingSurvival, token);
            token = -1;
            tokenHasSeparator = false;
            parsingBirth = true;
            parsingSurvival = false;
            continue;
        }

        if (ch == 'S' || ch == 's')
        {
            CommitNeighborToken(rules, parsingBirth, parsingSurvival, token);
            token = -1;
            tokenHasSeparator = false;
            parsingBirth = false;
            parsingSurvival = true;
            continue;
        }

        if (ch == '/')
        {
            CommitNeighborToken(rules, parsingBirth, parsingSurvival, token);
            token = -1;
            tokenHasSeparator = false;
            continue;
        }

        if (ch == ',' || ch == ';' || std::isspace(static_cast<unsigned char>(ch)) != 0)
        {
            CommitNeighborToken(rules, parsingBirth, parsingSurvival, token);
            token = -1;
            tokenHasSeparator = true;
            continue;
        }

        if (ch < '0' || ch > '9')
        {
            throw std::invalid_argument("Rule string contains invalid character.");
        }

        const int digit = ch - '0';
        if (tokenHasSeparator)
        {
            token = (token < 0) ? digit : token * 10 + digit;
            continue;
        }

        CommitNeighborToken(rules, parsingBirth, parsingSurvival, digit);
    }

    CommitNeighborToken(rules, parsingBirth, parsingSurvival, token);
    return rules;
}

bool RuleSet::ShouldLive(bool currentlyAlive, uint8_t neighborCount) const
{
    if (neighborCount >= 27)
    {
        return false;
    }

    return currentlyAlive ? survive[neighborCount] : birth[neighborCount];
}

bool ThresholdRange::Contains(float value) const
{
    return value >= min && value <= max;
}

WeightedRuleSet WeightedRuleSet::FromRuleSet(const RuleSet& rules)
{
    WeightedRuleSet weighted{};
    weighted.neighborWeights.fill(1.0f);
    weighted.boundary = rules.boundary;

    auto rangeFromCounts = [](const std::array<bool, 27>& counts) {
        ThresholdRange range{};
        float minValue = std::numeric_limits<float>::max();
        float maxValue = std::numeric_limits<float>::lowest();

        for (size_t i = 0; i < counts.size(); ++i)
        {
            if (counts[i])
            {
                minValue = std::min(minValue, static_cast<float>(i));
                maxValue = std::max(maxValue, static_cast<float>(i));
            }
        }

        if (minValue == std::numeric_limits<float>::max())
        {
            return range;
        }

        range.min = minValue;
        range.max = maxValue;
        return range;
    };

    weighted.birth = rangeFromCounts(rules.birth);
    weighted.survive = rangeFromCounts(rules.survive);
    return weighted;
}

WeightedRuleSet WeightedRuleSet::FromConfig(const SimulationConfig& config, BoundaryMode boundaryMode)
{
    WeightedRuleSet weighted = FromRuleSet(RuleSet::FromString(config.rule, boundaryMode));
    weighted.birth = {config.birthMin, config.birthMax};
    weighted.survive = {config.survivalMin, config.survivalMax};
    return weighted;
}

const std::array<glm::ivec3, 26>& WeightedRuleSet::NeighborOffsets()
{
    static const std::array<glm::ivec3, 26> offsets = [] {
        std::array<glm::ivec3, 26> values{};
        size_t index = 0;
        for (int32_t z = -1; z <= 1; ++z)
        {
            for (int32_t y = -1; y <= 1; ++y)
            {
                for (int32_t x = -1; x <= 1; ++x)
                {
                    if (x == 0 && y == 0 && z == 0)
                    {
                        continue;
                    }

                    values[index] = glm::ivec3{x, y, z};
                    ++index;
                }
            }
        }
        return values;
    }();

    return offsets;
}

bool WeightedRuleSet::ShouldLive(bool currentlyAlive, float weightedNeighborSum) const
{
    return currentlyAlive ? survive.Contains(weightedNeighborSum) : birth.Contains(weightedNeighborSum);
}
