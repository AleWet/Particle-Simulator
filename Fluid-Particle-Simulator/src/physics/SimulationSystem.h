#pragma once

#include <vector>
#include "Particle.h"
#include "glm/glm.hpp"

class SimulationSystem 
{ 
private:
	std::vector<Particle> m_Particles;
	glm::vec2 m_Bounds;

public:
    SimulationSystem(const glm::vec2& bounds);
    ~SimulationSystem();

    void AddParticle(const glm::vec2& position, float mass = 1.0f);
    void AddParticleGrid(int rows, int cols, const glm::vec2& topLeft,
        const glm::vec2& spacing, float mass = 1.0f);

    const std::vector<Particle>& GetParticles() const { return m_Particles; }
    std::vector<Particle>& GetParticles() { return m_Particles; }   // WHY DOES THIS WORK?

    const glm::vec2& GetBounds() const { return m_Bounds; }
};