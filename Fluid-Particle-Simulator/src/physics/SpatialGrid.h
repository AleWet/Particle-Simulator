#pragma once
#include <vector>
#include <utility>
#include "Vec2.h"

class SpatialGrid {
private:
    float m_CellSize;
    Vec2 m_MinBound;
    Vec2 m_MaxBound;
    int m_GridWidth;
    int m_GridHeight;
    std::vector<std::vector<int>> m_Grid;
    std::vector<std::pair<int, int>> m_CollisionPairs;
    int m_ParticleCount;

    // Neighbor offsets as pairs (dx, dy)
    static constexpr std::pair<int, int> NEIGHBOR_OFFSETS[3] = { {1, 0}, {1, 1}, {0, 1} };

    // Directly compute 1D cell index from position
    inline int GetCellIndex(const Vec2& position) const
    {
        int x = static_cast<int>((position.x - m_MinBound.x) / m_CellSize);
        x = (x < 0) ? 0 : ((x >= m_GridWidth) ? m_GridWidth - 1 : x);
        int y = static_cast<int>((position.y - m_MinBound.y) / m_CellSize);
        y = (y < 0) ? 0 : ((y >= m_GridHeight) ? m_GridHeight - 1 : y);
        return x + y * m_GridWidth;
    }

public:

    SpatialGrid(const Vec2& minBound, const Vec2& maxBound, float cellSize, int particleCount)
        : m_CellSize(cellSize), m_MinBound(minBound), m_MaxBound(maxBound), m_ParticleCount(particleCount)
    {
        m_GridWidth = static_cast<int>((maxBound.x - minBound.x) / cellSize) + 1;
        m_GridHeight = static_cast<int>((maxBound.y - minBound.y) / cellSize) + 1;
        m_Grid.resize(m_GridWidth * m_GridHeight);

        const int avgParticlesPerCell = std::max(1, particleCount / (m_GridWidth * m_GridHeight));
        for (auto& cell : m_Grid) {
            cell.reserve(avgParticlesPerCell * 2);
        }
    }

    void Clear()
    {
        for (auto& cell : m_Grid) {
            cell.clear();
        }
        m_CollisionPairs.clear();
    }

    inline bool AreParticlesCloseEnough(int a, int b, const std::vector<Particle>& particles, float maxDistance) const
    {
        const auto& posA = particles[a].position;
        const auto& posB = particles[b].position;
        const float dx = posA.x - posB.x;
        if (std::abs(dx) > maxDistance) return false;
        const float dy = posA.y - posB.y;
        if (std::abs(dy) > maxDistance) return false;
        return (dx * dx + dy * dy) <= maxDistance * maxDistance;
    }

    inline void InsertParticle(int particleIndex, const Vec2& position)
    {
        m_Grid[GetCellIndex(position)].push_back(particleIndex);
    }

    std::vector<std::pair<int, int>>& GetPotentialCollisionPairs(const std::vector<Particle>& particles, float maxDistance)
    {
        m_CollisionPairs.clear();
        m_CollisionPairs.reserve(m_ParticleCount * 6);

        for (int y = 0; y < m_GridHeight; ++y)
        {
            for (int x = 0; x < m_GridWidth; ++x)
            {
                const int cellIndex = x + y * m_GridWidth;
                const auto& cellParticles = m_Grid[cellIndex];
                if (cellParticles.empty()) continue;

                const size_t cellSize = cellParticles.size();
                for (size_t i = 0; i < cellSize; ++i)
                {
                    const int particleA = cellParticles[i];

                    // Intra-cell pairs
                    for (size_t j = i + 1; j < cellSize; ++j)
                    {
                        const int particleB = cellParticles[j];
                        if (AreParticlesCloseEnough(particleA, particleB, particles, maxDistance))
                        {
                            m_CollisionPairs.emplace_back(particleA, particleB);
                        }
                    }

                    // Neighbor cells
                    for (const auto& offset : NEIGHBOR_OFFSETS)
                    {
                        const int neighborX = x + offset.first;
                        const int neighborY = y + offset.second;
                        if (neighborX >= m_GridWidth || neighborY >= m_GridHeight) continue;

                        const int neighborIndex = neighborX + neighborY * m_GridWidth;
                        const auto& neighborParticles = m_Grid[neighborIndex];
                        if (neighborParticles.empty()) continue;

                        for (const int particleB : neighborParticles)
                        {
                            if (AreParticlesCloseEnough(particleA, particleB, particles, maxDistance))
                            {
                                m_CollisionPairs.emplace_back(particleA, particleB);
                            }
                        }
                    }
                }
            }
        }
        return m_CollisionPairs;
    }
};