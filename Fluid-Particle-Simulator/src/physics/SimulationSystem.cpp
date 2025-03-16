#include "SimulationSystem.h"
#include <iostream>
#include <algorithm>

SimulationSystem::SimulationSystem(const Vec2& bottomLeft, const Vec2& topRight, float particleRadius, unsigned int windowWidth)
    : m_Bounds({ bottomLeft, topRight }), m_ParticleRadius(particleRadius),
    m_Zoom(1.0f), m_WindowWidth(windowWidth)
{
    m_SimHeight = std::abs(topRight.y - bottomLeft.y);
    m_SimWidth = std::abs(topRight.x - bottomLeft.x);
}

SimulationSystem::~SimulationSystem()
{
    // Destructor implementation
    if (m_SpatialGrid) {
        delete m_SpatialGrid;
        m_SpatialGrid = nullptr;
    }
}

void SimulationSystem::AddParticle(const Vec2& position, const Vec2& velocity, float mass)
{
    Particle newParticle(position, velocity, mass);
    m_Particles.push_back(newParticle);
}

void SimulationSystem::AddParticleGrid(int rows, int cols, Vec2 spacing, bool withInitialVelocity, float mass)
{
    // Reserve memory at the start
    m_Particles.reserve(m_Particles.size() + rows * cols);

    // Calculate the starting position (top-left corner of the simulation area)
    float startX = m_Bounds.bottomLeft.x + m_ParticleRadius;
    float startY = m_Bounds.topRight.y - m_ParticleRadius;

    // Calculate step between particles (center-to-center distance)
    float stepX = 2.0f * m_ParticleRadius + spacing.x;
    float stepY = 2.0f * m_ParticleRadius + spacing.y;

    Vec2 vel = { 10.0, -10.0 };
    Vec2 nullVel = { 0.0, 0.0 };

    // Generate particles in a grid pattern
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Vec2 position(
                startX + j * stepX,          // Move right for each column
                startY - i * stepY           // Move down for each row
            );

            if (withInitialVelocity)
                AddParticle(position, vel, mass);
            else
                AddParticle(position, nullVel, mass);
        }
    }
}

glm::mat4 SimulationSystem::GetProjMatrix() const
{
    // Calculate the simulation boundaries
    const float simWidth = m_Bounds.topRight.x - m_Bounds.bottomLeft.x;
    const float simHeight = m_Bounds.topRight.y - m_Bounds.bottomLeft.y;

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
    // Calculate simulation center 
    Vec2 simulationCenter = {
        (m_Bounds.topRight.x + m_Bounds.bottomLeft.x) * 0.5f,
        (m_Bounds.topRight.y + m_Bounds.bottomLeft.y) * 0.5f
    };

    // Create view transformation matrix
    glm::mat4 view = glm::mat4(1.0f);

    // Center the view on the simulation area
    view = glm::translate(view, glm::vec3(
        -simulationCenter.x,  // Center X
        -simulationCenter.y,  // Center Y
        0.0f                  // Z remains unchanged
    ));

    return view;
}

void SimulationSystem::AddParticleStream(int totalParticles, float spawnRate, const Vec2& velocity,
    float mass, const Vec2& initialOffset)
{
    ParticleStream newStream;
    newStream.isActive = true;
    newStream.startPos =
    {
        m_Bounds.bottomLeft.x + m_ParticleRadius + initialOffset.x,
        m_Bounds.topRight.y - m_ParticleRadius - initialOffset.y
    };
    newStream.velocity = velocity;
    newStream.total = totalParticles;
    newStream.spawnInterval = 1.0f / spawnRate;
    newStream.timer = 0.0f;
    newStream.spawned = 0;
    newStream.mass = mass;

    m_Streams.push_back(newStream);
}
void SimulationSystem::UpdateStreams(float deltaTime)
{
    // Use iterator to allow removing completed streams if needed
    for (auto& stream : m_Streams) {
        if (!stream.isActive || stream.spawned >= stream.total) continue;

        stream.timer += deltaTime;

        while (stream.timer >= stream.spawnInterval && stream.spawned < stream.total) {
            AddParticle(stream.startPos, stream.velocity, stream.mass);
            stream.spawned++;
            stream.timer -= stream.spawnInterval;
        }
    }
}

void SimulationSystem::InitSpatialGrid()
{
    if (m_SpatialGrid) {
        delete m_SpatialGrid;
    }

    // size should be slightly larger than twice the particle diameter
    float cellSize = 3.1f * 2.0f * m_ParticleRadius;
    const auto& bounds = GetBounds();
    m_SpatialGrid = new SpatialGrid(bounds.bottomLeft, bounds.topRight, cellSize, m_Particles.size());
}