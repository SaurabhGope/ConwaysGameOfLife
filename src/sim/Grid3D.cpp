#include "sim/Grid3D.hpp"

#include <stdexcept>

Grid3D::Grid3D(glm::uvec3 dims)
    : m_dims(dims)
{
    const uint64_t cellCount = CellCount();
    const uint64_t wordCount = (cellCount + 63ull) / 64ull;
    m_words.assign(wordCount, 0ull);
}

glm::uvec3 Grid3D::Dims() const
{
    return m_dims;
}

uint64_t Grid3D::CellCount() const
{
    return static_cast<uint64_t>(m_dims.x) *
        static_cast<uint64_t>(m_dims.y) *
        static_cast<uint64_t>(m_dims.z);
}

bool Grid3D::Get(uint32_t x, uint32_t y, uint32_t z) const
{
    if (!InBounds(x, y, z))
    {
        throw std::out_of_range("Grid3D::Get out of range.");
    }

    const uint64_t index = LinearIndex(x, y, z);
    const auto [wordIndex, mask] = WordAndMask(index);
    return (m_words[wordIndex] & mask) != 0ull;
}

void Grid3D::Set(uint32_t x, uint32_t y, uint32_t z, bool alive)
{
    if (!InBounds(x, y, z))
    {
        throw std::out_of_range("Grid3D::Set out of range.");
    }

    const uint64_t index = LinearIndex(x, y, z);
    const auto [wordIndex, mask] = WordAndMask(index);
    const bool currentlyAlive = (m_words[wordIndex] & mask) != 0ull;

    if (currentlyAlive == alive)
    {
        return;
    }

    if (alive)
    {
        m_words[wordIndex] |= mask;
        ++m_aliveCount;
    }
    else
    {
        m_words[wordIndex] &= ~mask;
        --m_aliveCount;
    }
}

void Grid3D::Fill(bool alive)
{
    if (m_words.empty())
    {
        m_aliveCount = 0;
        return;
    }

    const uint64_t fillWord = alive ? ~0ull : 0ull;
    for (auto& word : m_words)
    {
        word = fillWord;
    }

    const uint64_t cellCount = CellCount();
    const uint64_t remainder = cellCount % 64ull;
    if (alive && remainder != 0ull)
    {
        const uint64_t validBitsMask = (1ull << remainder) - 1ull;
        m_words.back() &= validBitsMask;
    }

    m_aliveCount = alive ? cellCount : 0ull;
}

void Grid3D::Clear()
{
    Fill(false);
}

uint64_t Grid3D::AliveCount() const
{
    return m_aliveCount;
}

uint64_t Grid3D::LinearIndex(uint32_t x, uint32_t y, uint32_t z) const
{
    if (!InBounds(x, y, z))
    {
        throw std::out_of_range("Grid3D::LinearIndex out of range.");
    }

    return static_cast<uint64_t>(z) * static_cast<uint64_t>(m_dims.y) * static_cast<uint64_t>(m_dims.x) +
        static_cast<uint64_t>(y) * static_cast<uint64_t>(m_dims.x) +
        static_cast<uint64_t>(x);
}

const std::vector<uint64_t>& Grid3D::Words() const
{
    return m_words;
}

bool Grid3D::InBounds(uint32_t x, uint32_t y, uint32_t z) const
{
    return x < m_dims.x && y < m_dims.y && z < m_dims.z;
}

std::pair<uint64_t, uint64_t> Grid3D::WordAndMask(uint64_t linearIndex) const
{
    const uint64_t wordIndex = linearIndex / 64ull;
    const uint64_t bitIndex = linearIndex % 64ull;
    const uint64_t mask = 1ull << bitIndex;
    return {wordIndex, mask};
}
