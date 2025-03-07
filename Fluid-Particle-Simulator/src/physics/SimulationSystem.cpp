#include "SimulationSystem.h"
#include <iostream>
#include <algorithm>

SimulationSystem::SimulationSystem(const glm::vec2& bottomLeft, const glm::vec2& topRight, unsigned int particleRadius)
    : m_bottomLeft(bottomLeft), m_topRight(topRight), m_ParticleRadius(particleRadius)
{
}

SimulationSystem::~SimulationSystem()
{
    // Destructor implementation (empty for now)
}

void SimulationSystem::AddParticle(const glm::vec2& position, float mass)
{
    Particle newParticle(position, mass);
    m_Particles.push_back(newParticle);
}

void SimulationSystem::AddParticleGrid(int rows, int cols, const glm::vec2& bottomLeft, const glm::vec2& topRight,
    const glm::vec2& spacing, float mass)
{
    if (rows <= 0 || cols <= 0) {
        std::cout << "Error in AddParticleGrid: rows or columns are negative or 0" << std::endl;
        return;
    }

    // Constrain the grid area to be within simulation bounds
    glm::vec2 gridBottomLeft = glm::vec2(
        std::max(bottomLeft.x, m_bottomLeft.x),
        std::max(bottomLeft.y, m_bottomLeft.y)
    );

    glm::vec2 gridTopRight = glm::vec2(
        std::min(topRight.x, m_topRight.x),
        std::min(topRight.y, m_topRight.y)
    );

    // Calculate available space and adjust spacing if needed
    float availWidth = gridTopRight.x - gridBottomLeft.x;
    float availHeight = gridTopRight.y - gridBottomLeft.y;

    // Ensure we have enough space for particles + spacing
    float effectiveWidth = availWidth - 2 * m_ParticleRadius;
    float effectiveHeight = availHeight - 2 * m_ParticleRadius;

    if (effectiveWidth <= 0 || effectiveHeight <= 0) {
        std::cout << "Error in AddParticleGrid: Not enough space for particles" << std::endl;
        return;
    }

    // Calculate actual spacing between particles
    float xSpacing = (cols > 1) ? effectiveWidth / (cols - 1) : 0;
    float ySpacing = (rows > 1) ? effectiveHeight / (rows - 1) : 0;

    // If user provided spacing, use the minimum between available and requested
    if (spacing.x > 0) xSpacing = std::min(xSpacing, spacing.x);
    if (spacing.y > 0) ySpacing = std::min(ySpacing, spacing.y);

    // Starting position for the first particle (bottom-left of the grid)
    glm::vec2 startPos = gridBottomLeft + glm::vec2(m_ParticleRadius, m_ParticleRadius);

    // Create the grid of particles
    for (int col = 0; col < cols; col++) {
        for (int row = 0; row < rows; row++) {
            glm::vec2 position = startPos + glm::vec2(col * xSpacing, row * ySpacing);
            AddParticle(position, mass);
        }
    }
}

glm::mat4 SimulationSystem::GetViewMatrix(float zoom, float aspectRatio)     const 
{
    // Calculate the width and height of the simulation
    float simWidth = m_topRight.x - m_bottomLeft.x;
    float simHeight = m_topRight.y - m_bottomLeft.y;

    // Calculate the center of the simulation
    glm::vec2 center = (m_topRight + m_bottomLeft) * 0.5f;

    // Adjust for zoom factor
    float viewWidth = simWidth / zoom;
    float viewHeight = viewWidth / aspectRatio;

    // Create orthographic projection matrix centered on the simulation
    glm::mat4 projection = glm::ortho(
        center.x - viewWidth / 2, center.x + viewWidth / 2,   // left, right
        center.y - viewHeight / 2, center.y + viewHeight / 2, // bottom, top
        -1.0f, 1.0f                                           // near, far
    );


    return projection;
}