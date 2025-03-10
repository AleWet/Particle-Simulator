#include "SimulationSystem.h"
#include <iostream>
#include <algorithm>

SimulationSystem::SimulationSystem(const glm::vec2& bottomLeft, const glm::vec2& topRight, unsigned int particleRadius, unsigned int windowWidth, float simulationBorderOffset)
    : m_bottomLeft(bottomLeft), m_topRight(topRight), m_ParticleRadius(particleRadius),
    m_Zoom(1.0f), m_WindowWidth(windowWidth), m_SimulationBorderOffSet(simulationBorderOffset)
{
    m_SimHeight = std::abs(topRight.y - bottomLeft.y);
    m_SimWidth = std::abs(topRight.x - bottomLeft.x);
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

void SimulationSystem::AddParticleGrid(int rows, int cols, glm::vec2 spacing, float mass)
{
    // Calculate the starting position (top-left corner of the simulation area)
    float startX = m_bottomLeft.x + 2 * m_ParticleRadius;
    float startY = m_topRight.y - 2 * m_ParticleRadius;

    // Calculate step between particles (center-to-center distance)
    float stepX = 3.0f * m_ParticleRadius + spacing.x; // the three is a bug that i have yet to figure out 
    float stepY = 3.0f * m_ParticleRadius + spacing.y;

    // Generate particles in a grid pattern
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            glm::vec2 position(
                startX + j * stepX,          // Move right for each column
                startY - i * stepY           // Move down for each row
            );
            AddParticle(position, mass);
        }
    }
}

glm::mat4 SimulationSystem::GetProjMatrix() const
{
    // Calculate the simulation boundaries
    const float simWidth = m_topRight.x - m_bottomLeft.x;
    const float simHeight = m_topRight.y - m_bottomLeft.y;

    // Calculate the aspect ratio of the simulation space itself
    const float simulationAspectRatio = simWidth / simHeight;

    // Base projection dimensions on simulation's native aspect ratio
    const float baseWidth = simWidth / m_Zoom;
    const float baseHeight = baseWidth / simulationAspectRatio;

    // Create orthographic projection that matches the simulation's aspect ratio
    return glm::ortho(
        -baseWidth / 2.0f, baseWidth / 2.0f,  // Left/Right
        -baseHeight / 2.0f, baseHeight / 2.0f,  // Bottom/Top
        -1.0f, 1.0f                             // Near/Far (Z-clipping)
    );
}

glm::mat4 SimulationSystem::GetViewMatrix() const
{
    // Calculate simulation center point
    const glm::vec2 simulationCenter = (m_topRight + m_bottomLeft) * 0.5f;

    // Create view transformation matrix
    glm::mat4 view = glm::mat4(1.0f);

    // Apply vertical offset to create top border
    view = glm::translate(view, glm::vec3(
        0.0f,                       // X: No horizontal translation
        -m_SimulationBorderOffSet,  // Y: Move "camera" down to create top border
        0.0f                        // Z: No depth translation
    ));

    // Center the view on the simulation area
    view = glm::translate(view, glm::vec3(
        -simulationCenter.x,  // Center X
        -simulationCenter.y,  // Center Y
        0.0f                  // Z remains unchanged
    ));

    return view;
}

float SimulationSystem::GetParticleRenderSize() const
{
    // Calculate the simulation width in simulation units
    float simWidth = m_topRight.x - m_bottomLeft.x;

    // Calculate simulation units per pixel
    float scale = simWidth / static_cast<float>(m_WindowWidth);

    // Convert particle radius (sim units) to pixels
    return m_ParticleRadius / scale; 
}
