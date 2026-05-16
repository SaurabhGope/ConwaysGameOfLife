#pragma once

#include "seed/SeedTypes.hpp"

class ISeedGenerator
{
public:
    virtual ~ISeedGenerator() = default;
    virtual SeedType Type() const = 0;
    virtual SeedMask Generate(const SeedRequest& request) const = 0;
};
