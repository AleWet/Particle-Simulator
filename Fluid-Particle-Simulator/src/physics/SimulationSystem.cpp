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

void SimulationSystem::AddParticleGrid(int rows, int cols, const glm::vec2& bottomLeft, const glm::vec2& topRight,
    glm::vec2& spacing, float mass)
{
    // Validate input parameters
    if (rows <= 0 || cols <= 0) {
        std::cerr << "Error: Cannot create grid with " << rows << " rows and "
            << cols << " columns. Values must be positive." << std::endl;
        return;
    }

    // Ensure grid boundaries are within simulation bounds
    glm::vec2 gridBottomLeft = glm::max(bottomLeft, m_bottomLeft + glm::vec2(m_ParticleRadius));
    glm::vec2 gridTopRight = glm::min(topRight, m_topRight - glm::vec2(m_ParticleRadius));

    // Check if we have enough space for at least one particle
    if (gridBottomLeft.x >= gridTopRight.x || gridBottomLeft.y >= gridTopRight.y) {
        std::cerr << "Error: Not enough space within bounds to create particle grid" << std::endl;
        return;
    }

    // Calculate grid dimensions
    glm::vec2 gridSize = gridTopRight - gridBottomLeft;

    // Check if minimum grid can fit (one particle in each corner)
    if (gridSize.x < 2 * m_ParticleRadius || gridSize.y < 2 * m_ParticleRadius) {
        std::cerr << "Error: Grid area too small for particles of radius " << m_ParticleRadius << std::endl;
        return;
    }

    // Calculate available space for particles
    float availableWidth = gridSize.x - 2 * m_ParticleRadius;  // Account for particles at edges
    float availableHeight = gridSize.y - 2 * m_ParticleRadius;

    // Calculate default spacing if available space permits
    glm::vec2 finalSpacing = spacing;

    // Check if we need cols-1 spaces in X direction and rows-1 spaces in Y direction
    if (cols > 1) {
        float totalWidthNeeded = 2 * m_ParticleRadius * cols + spacing.x * (cols - 1);
        if (totalWidthNeeded > gridSize.x) {
            // Spacing is too large, calculate maximum possible spacing
            float maxSpacingX = (gridSize.x - 2 * m_ParticleRadius * cols) / (cols - 1);
            if (maxSpacingX < 0) {
                // Can't fit with any spacing, try without spacing
                finalSpacing.x = 0;
                std::cout << "Warning: X spacing reduced to 0 to fit particles" << std::endl;
            }
            else {
                finalSpacing.x = maxSpacingX;
                std::cout << "Warning: X spacing reduced to " << maxSpacingX << " to fit particles" << std::endl;
            }
        }
    }

    if (rows > 1) {
        float totalHeightNeeded = 2 * m_ParticleRadius * rows + spacing.y * (rows - 1);
        if (totalHeightNeeded > gridSize.y) {
            // Spacing is too large, calculate maximum possible spacing
            float maxSpacingY = (gridSize.y - 2 * m_ParticleRadius * rows) / (rows - 1);
            if (maxSpacingY < 0) {
                // Can't fit with any spacing, try without spacing
                finalSpacing.y = 0;
                std::cout << "Warning: Y spacing reduced to 0 to fit particles" << std::endl;
            }
            else {
                finalSpacing.y = maxSpacingY;
                std::cout << "Warning: Y spacing reduced to " << maxSpacingY << " to fit particles" << std::endl;
            }
        }
    }

    // Check if particles would overlap with zero spacing
    if (2 * m_ParticleRadius * cols > gridSize.x || 2 * m_ParticleRadius * rows > gridSize.y) {
        std::cerr << "Error: Cannot fit " << rows << "x" << cols << " particles even with zero spacing" << std::endl;
        return;
    }

    // Calculate step sizes between particles
    float stepX = (cols > 1) ? (2 * m_ParticleRadius + finalSpacing.x) : 0;
    float stepY = (rows > 1) ? (2 * m_ParticleRadius + finalSpacing.y) : 0;

    // Position of first particle (top-left corner with particle tangent to borders)
    glm::vec2 startPos = glm::vec2(
        gridBottomLeft.x + m_ParticleRadius,  // Left edge + radius = tangent to left border
        gridTopRight.y - m_ParticleRadius     // Top edge - radius = tangent to top border
    );

    // Generate the particles
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            // Calculate position for this particle
            glm::vec2 position(
                startPos.x + col * stepX,
                startPos.y - row * stepY  // Subtract for Y since we start from top
            );

            // Collision detection with existing particles
            bool collisionDetected = false;
            for (const Particle& existingParticle : m_Particles) {
                float distance = glm::length(existingParticle.position - position);
                if (distance < 2 * m_ParticleRadius) {
                    collisionDetected = true;
                    std::cout << "Warning: Skipping particle at position (" << position.x << ", "
                        << position.y << ") due to collision with existing particle" << std::endl;
                    break;
                }
            }

            // Add the particle if no collision
            if (!collisionDetected) {
                AddParticle(position, mass);
            }
        }
    }

    std::cout << "Created grid with " << rows << "x" << cols << " particles" << std::endl;
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

    // Don't make this function depend on the zoom level, it's already applied with the viewMatrix
    // and since this is all relative to the simulation coordinates 

    float viewWidth = simWidth;

    // Calculate simulation units per pixel
    float scale = viewWidth / static_cast<float>(m_WindowWidth);

    // Convert pixel distance to simulation distance
    return m_ParticleRadius * scale;
}
