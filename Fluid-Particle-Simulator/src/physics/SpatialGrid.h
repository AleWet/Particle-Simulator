#pragma once
#include <vector>
#include <glm/glm.hpp>


class SpatialGrid {
private:
    float m_CellSize;
    glm::vec2 m_MinBound;   // Bottom-left of simulation area
    glm::vec2 m_MaxBound;   // Top-right of simulation area
    int m_GridWidth;        // Number of cells along X-axis
    int m_GridHeight;       // Number of cells along Y-axis
    std::vector<std::vector<int>> m_Grid;  // 1D grid (flattened 2D)
    std::vector<std::pair<int, int>> m_CollisionPairs; // Cache collision pairs vector
    int m_ParticleCount;    

    // Converts a world position to grid indices
    glm::ivec2 GetCellIndices(const glm::vec2& position) const 
    {
        int x = static_cast<int>((position.x - m_MinBound.x) / m_CellSize);
        int y = static_cast<int>((position.y - m_MinBound.y) / m_CellSize);

        // Clamp to grid boundaries
        if (x < 0) x = 0;
        else if (x >= m_GridWidth) x = m_GridWidth - 1;
        if (y < 0) y = 0;
        else if (y >= m_GridHeight) y = m_GridHeight - 1;
        return glm::ivec2(x, y);
    }

    // Converts 2D grid indices to a 1D index
    int To1DIndex(const glm::ivec2& cell) const 
    {
        return cell.x + cell.y * m_GridWidth;
    }

public:
    // Constructor with simulation bounds and cell size
    SpatialGrid(const glm::vec2& minBound, const glm::vec2& maxBound, float cellSize, int particleCount)
        : m_CellSize(cellSize), m_MinBound(minBound), m_MaxBound(maxBound), m_ParticleCount(particleCount)
    {
        // Calculate grid dimensions
        m_GridWidth = static_cast<int>((maxBound.x - minBound.x) / cellSize) + 1;
        m_GridHeight = static_cast<int>((maxBound.y - minBound.y) / cellSize) + 1;

        // Initialize grid with empty vectors
        m_Grid.resize(m_GridWidth * m_GridHeight);
    }

    // Clear all cells
    void Clear() 
    {
        for (auto& cell : m_Grid) {
            // Empty cells but retain memory
            cell.clear();
        }
    }

    // Check if it makes sense to add particle pair (a,b) to the physics check
    inline bool AreParticlesCloseEnough(int a, int b, const std::vector<Particle>& particles, float maxDistance)
    {
        const auto& posA = particles[a].position;
        const auto& posB = particles[b].position;

        // Fast squared distance check (avoid sqrt)
        float dx = posA.x - posB.x;
        float dy = posA.y - posB.y;
        float distSquared = dx * dx + dy * dy;

        return distSquared <= maxDistance * maxDistance;
    }

    // Insert a particle into the grid
    void InsertParticle(int particleIndex, const glm::vec2& position)
    {
        glm::ivec2 cell = GetCellIndices(position);
        m_Grid[To1DIndex(cell)].push_back(particleIndex);
    }

    std::vector<std::pair<int, int>>& GetPotentialCollisionPairs(const std::vector<Particle>& particles, float maxDistance)
    {
        // Clear previous results but keep capacity
        m_CollisionPairs.clear();
        // Pre-allocate memory to avoid reallocations
        m_CollisionPairs.reserve(m_ParticleCount * 4);

        // Check only these neighbors to avoid duplicates
        const glm::ivec2 neighbors[] = { {1, 0}, {1, 1}, {0, 1}};

        for (int y = 0; y < m_GridHeight; ++y)
        {
            for (int x = 0; x < m_GridWidth; ++x)
            {
                const glm::ivec2 currentCell(x, y);
                const int cellIndex = To1DIndex(currentCell);
                const auto& cellParticles = m_Grid[cellIndex];

                // Intra-cell pairs (particles within the same cell)
                for (size_t i = 0; i < cellParticles.size(); ++i)
                {
                    for (size_t j = i + 1; j < cellParticles.size(); ++j)
                    {
                        int particleA = cellParticles[i];
                        int particleB = cellParticles[j];

                        // Verify particles are within interaction distance
                        if (AreParticlesCloseEnough(particleA, particleB, particles, maxDistance))
                        {
                            m_CollisionPairs.emplace_back(particleA, particleB);
                        }
                    }
                }

                // Inter-cell pairs (particles from neighboring cells)
                for (const auto& offset : neighbors)
                {
                    const glm::ivec2 neighborCell = currentCell + offset;
                    if (neighborCell.x >= 0 && neighborCell.x < m_GridWidth &&
                        neighborCell.y >= 0 && neighborCell.y < m_GridHeight)
                    {
                        const int neighborIndex = To1DIndex(neighborCell);
                        const auto& neighborParticles = m_Grid[neighborIndex];

                        // Cross-check all particles between current cell and neighbor cell
                        for (int particleA : cellParticles)
                        {
                            for (int particleB : neighborParticles)
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
        }

        return m_CollisionPairs;
    }
};