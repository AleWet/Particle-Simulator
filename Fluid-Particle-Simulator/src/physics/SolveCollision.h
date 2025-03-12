#pragma once
#include "SimulationSystem.h"

// Solve collision between particle (particleA) and simulation 
void SolveCollisionBorder(Particle& particleA,
    const std::vector<glm::vec2> simBounds,
    float particleRadius);

// Solve collision between particle A and particle B.
// At the moment this uses a naive approach but will be improved
void SolveCollisionParticle(Particle& particleA, Particle& particleB,
    const std::vector<glm::vec2> simBounds,
    float particleRadius);