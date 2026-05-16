#include "seed/SeedEngine.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>
#include <utility>
#include "core/Logger.hpp"
#include <glm/geometric.hpp>

namespace
{
uint64_t SeedIndex(uint32_t x, uint32_t y, uint32_t z, glm::uvec3 dims)
{
    return static_cast<uint64_t>(z) * dims.y * dims.x +
        static_cast<uint64_t>(y) * dims.x +
        static_cast<uint64_t>(x);
}

class CubeSeedGenerator final : public ISeedGenerator
{
public:
    SeedType Type() const override { return SeedType::Cube; }

    SeedMask Generate(const SeedRequest& request) const override
    {
        SeedMask mask{request.transform.size, {}};
        mask.voxels.assign(static_cast<size_t>(mask.dims.x) * mask.dims.y * mask.dims.z, 1u);
        return mask;
    }
};

class SphereSeedGenerator final : public ISeedGenerator
{
public:
    SeedType Type() const override { return SeedType::Sphere; }

    SeedMask Generate(const SeedRequest& request) const override
    {
        SeedMask mask{request.transform.size, {}};
        mask.voxels.assign(static_cast<size_t>(mask.dims.x) * mask.dims.y * mask.dims.z, 0u);

        const glm::vec3 center = (glm::vec3(mask.dims) - glm::vec3(1.0f)) * 0.5f;
        const float radius = std::max(1.0f, static_cast<float>(std::min({mask.dims.x, mask.dims.y, mask.dims.z})) * 0.5f);

        for (uint32_t z = 0; z < mask.dims.z; ++z)
        {
            for (uint32_t y = 0; y < mask.dims.y; ++y)
            {
                for (uint32_t x = 0; x < mask.dims.x; ++x)
                {
                    const glm::vec3 p{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
                    if (glm::length(p - center) <= radius)
                    {
                        mask.voxels[static_cast<size_t>(SeedIndex(x, y, z, mask.dims))] = 1u;
                    }
                }
            }
        }

        return mask;
    }
};

class LineSeedGenerator final : public ISeedGenerator
{
public:
    SeedType Type() const override { return SeedType::Line; }

    SeedMask Generate(const SeedRequest& request) const override
    {
        SeedMask mask{request.transform.size, {}};
        mask.voxels.assign(static_cast<size_t>(mask.dims.x) * mask.dims.y * mask.dims.z, 0u);

        const uint32_t y = mask.dims.y / 2u;
        const uint32_t z = mask.dims.z / 2u;
        for (uint32_t x = 0; x < mask.dims.x; ++x)
        {
            mask.voxels[static_cast<size_t>(SeedIndex(x, y, z, mask.dims))] = 1u;
        }

        return mask;
    }
};

class NoiseSeedGenerator final : public ISeedGenerator
{
public:
    SeedType Type() const override { return SeedType::Noise; }

    SeedMask Generate(const SeedRequest& request) const override
    {
        SeedMask mask{request.transform.size, {}};
        mask.voxels.assign(static_cast<size_t>(mask.dims.x) * mask.dims.y * mask.dims.z, 0u);

        std::mt19937 rng(request.deterministicSeed);
        std::bernoulli_distribution alive(std::clamp(request.transform.density, 0.0f, 1.0f));
        for (auto& voxel : mask.voxels)
        {
            voxel = alive(rng) ? 1u : 0u;
        }

        return mask;
    }
};
}

SeedEngine::SeedEngine()
{
    Register(std::make_unique<CubeSeedGenerator>());
    Register(std::make_unique<SphereSeedGenerator>());
    Register(std::make_unique<LineSeedGenerator>());
    Register(std::make_unique<NoiseSeedGenerator>());
    LIFE3D_LOG_DEBUG("seed", "Default seed generators registered.");
}

void SeedEngine::Register(std::unique_ptr<ISeedGenerator> generator)
{
    if (generator != nullptr)
    {
        LIFE3D_LOG_TRACE("seed", "Registering seed generator {}.", static_cast<int>(generator->Type()));
        m_generators.push_back(std::move(generator));
    }
}

std::optional<SeedMask> SeedEngine::Preview(const SeedRequest& request) const
{
    for (const auto& generator : m_generators)
    {
        if (generator->Type() == request.type)
        {
            return generator->Generate(request);
        }
    }

    LIFE3D_LOG_WARN("seed", "No seed generator registered for type {}.", static_cast<int>(request.type));
    return std::nullopt;
}

bool SeedEngine::Apply(const SeedRequest& request, Grid3D& grid) const
{
    const auto mask = Preview(request);
    if (!mask.has_value())
    {
        LIFE3D_LOG_WARN("seed", "Seed apply failed because preview returned no mask.");
        return false;
    }

    const auto dims = grid.Dims();
    const auto& seedMask = mask.value();

    for (uint32_t z = 0; z < seedMask.dims.z; ++z)
    {
        for (uint32_t y = 0; y < seedMask.dims.y; ++y)
        {
            for (uint32_t x = 0; x < seedMask.dims.x; ++x)
            {
                const uint64_t localIndex =
                    static_cast<uint64_t>(z) * seedMask.dims.y * seedMask.dims.x +
                    static_cast<uint64_t>(y) * seedMask.dims.x +
                    static_cast<uint64_t>(x);

                if (seedMask.voxels[localIndex] == 0u)
                {
                    continue;
                }

                const int32_t gx = request.transform.position.x + static_cast<int32_t>(x);
                const int32_t gy = request.transform.position.y + static_cast<int32_t>(y);
                const int32_t gz = request.transform.position.z + static_cast<int32_t>(z);

                if (gx < 0 || gy < 0 || gz < 0)
                {
                    continue;
                }

                if (gx >= static_cast<int32_t>(dims.x) ||
                    gy >= static_cast<int32_t>(dims.y) ||
                    gz >= static_cast<int32_t>(dims.z))
                {
                    continue;
                }

                grid.Set(static_cast<uint32_t>(gx), static_cast<uint32_t>(gy), static_cast<uint32_t>(gz), true);
            }
        }
    }
    LIFE3D_LOG_DEBUG(
        "seed",
        "Seed applied. type={} mask={}x{}x{} grid_alive={}",
        static_cast<int>(request.type),
        seedMask.dims.x,
        seedMask.dims.y,
        seedMask.dims.z,
        grid.AliveCount());
    return true;
}
