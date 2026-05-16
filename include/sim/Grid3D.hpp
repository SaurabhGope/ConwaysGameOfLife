#pragma once

#include <cstdint>
#include <vector>
#include <glm/vec3.hpp>

class Grid3D
{
public:
    explicit Grid3D(glm::uvec3 dims);

    glm::uvec3 Dims() const;
    uint64_t CellCount() const;

    bool Get(uint32_t x, uint32_t y, uint32_t z) const;
    void Set(uint32_t x, uint32_t y, uint32_t z, bool alive);
    void Fill(bool alive);
    void Clear();

    uint64_t AliveCount() const;
    uint64_t LinearIndex(uint32_t x, uint32_t y, uint32_t z) const;

    const std::vector<uint64_t>& Words() const;

private:
    bool InBounds(uint32_t x, uint32_t y, uint32_t z) const;
    std::pair<uint64_t, uint64_t> WordAndMask(uint64_t linearIndex) const;

    glm::uvec3 m_dims{};
    std::vector<uint64_t> m_words;
    uint64_t m_aliveCount = 0;
};
