#pragma once

#include <cstdint>
#include <vector>
#include <glm/vec3.hpp>

enum class SeedType
{
    Cube,
    Sphere,
    Line,
    Noise,
    Custom
};

struct SeedTransform
{
    glm::ivec3 position{0, 0, 0};
    glm::vec3 eulerDegrees{0.0f, 0.0f, 0.0f};
    glm::uvec3 size{8, 8, 8};
    float density = 1.0f;
};

struct SeedRequest
{
    SeedType type = SeedType::Cube;
    uint32_t deterministicSeed = 1;
    SeedTransform transform;
};

struct SeedMask
{
    glm::uvec3 dims{0, 0, 0};
    std::vector<uint8_t> voxels;
};
