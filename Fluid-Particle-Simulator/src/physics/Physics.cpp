#include "physics.h"

const glm::vec2 G = glm::vec2(0.0f, -9.80665f);

void UpdatePhysics(SimulationSystem& sim, float deltaTime)
{
    // Use reference to modify the actual particles
    std::vector<Particle>& particles = sim.GetParticles();
    int N = particles.size();

    for (int i = 0; i < N; i++)
    {
        // Reference to the current particle to modify it directly
        Particle& particleA = particles[i];

        // assume only grav. force
        particleA.force = particleA.mass * G;

        // Apply simple damping (air resistance)
        particleA.force -= particleA.velocity * 0.1f;

        // Update velocity based on force
        particleA.velocity += (particleA.force / particleA.mass) * deltaTime;

        // Update position
        particleA.position += particleA.velocity * deltaTime;

        // Add a small amount of temperature for fast-moving particles kinda like friction 
        float speed = glm::length(particleA.velocity);
        if (speed > 5.0f) {
            particleA.temperature = std::min(100.0f, particleA.temperature + 0.1f);
        }
        else {
            // Cool down
            particleA.temperature = std::max(20.0f, particleA.temperature - 0.05f);
        }

        glm::vec4 color(0.0f);

        // Solve collision between particle and border
        SolveCollisionBorder(particleA, sim.GetBounds(), sim.GetParticleRadius());
        
        // Solve collision between particle and particle
        for (int j = 0; j < N; j++)
        {            
            if (j != i)
            {
                Particle& particleB = particles[j];
                SolveCollisionParticle(particleA, particleB, sim.GetBounds(), sim.GetParticleRadius());
            }
        }
    }
}