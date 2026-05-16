#pragma once

#include <memory>
#include <optional>
#include <vector>
#include "seed/ISeedGenerator.hpp"
#include "sim/Grid3D.hpp"

class SeedEngine
{
public:
    SeedEngine();

    void Register(std::unique_ptr<ISeedGenerator> generator);
    std::optional<SeedMask> Preview(const SeedRequest& request) const;
    bool Apply(const SeedRequest& request, Grid3D& grid) const;

private:
    std::vector<std::unique_ptr<ISeedGenerator>> m_generators;
};
