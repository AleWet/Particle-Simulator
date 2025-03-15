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
    int m_ParticleCount;

    // Converts a world position to grid indices
    glm::ivec2 GetCellIndices(const glm::vec2& position) const {
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
    int To1DIndex(const glm::ivec2& cell) const {
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
    void Clear() {
        for (auto& cell : m_Grid) {
            // Empty cells but retain memory
            cell.clear();
        }
    }

    // Insert a particle into the grid
    void InsertParticle(int particleIndex, const glm::vec2& position)
    {
        glm::ivec2 cell = GetCellIndices(position);
        m_Grid[To1DIndex(cell)].push_back(particleIndex);
    }

    std::vector<std::pair<int, int>> GetPotentialCollisionPairs()
    {
        // Estimate pairs: 4 neighbors * avg 10 particles per cell * 4 interactions
        const size_t estimatedPairs = m_ParticleCount * 4;
        std::vector<std::pair<int, int>> pairs;
        pairs.reserve(estimatedPairs); // Single allocation upfront

        // Check only these neighbors to avoid duplicates
        const glm::ivec2 neighbors[] = { {1, 0}, {1, 1}, {0, 1} };

        for (int y = 0; y < m_GridHeight; ++y)
        {
            for (int x = 0; x < m_GridWidth; ++x)
            {
                const glm::ivec2 currentCell(x, y);
                const int cellIndex = To1DIndex(currentCell);
                const auto& particles = m_Grid[cellIndex];

                // Intra-cell pairs (i < j to avoid duplicates)
                for (size_t i = 0; i < particles.size(); ++i)
                {
                    for (size_t j = i + 1; j < particles.size(); ++j)
                    {
                        pairs.emplace_back(particles[i], particles[j]);
                    }
                }

                // Inter-cell pairs (no duplicates)
                for (const auto& offset : neighbors)
                {
                    const glm::ivec2 neighborCell = currentCell + offset;
                    if (neighborCell.x >= 0 && neighborCell.x < m_GridWidth &&
                        neighborCell.y >= 0 && neighborCell.y < m_GridHeight)
                    {
                        const int neighborIndex = To1DIndex(neighborCell);
                        const auto& neighborParticles = m_Grid[neighborIndex];

                        // Cross-check all particles
                        for (int a : particles)
                        {
                            for (int b : neighborParticles)
                            {
                                pairs.emplace_back(a, b);
                            }
                        }
                    }
                }
            }
        }

        return pairs;
    }
};