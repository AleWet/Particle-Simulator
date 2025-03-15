#include "physics.h"
#include "SpatialGrid.h"

const glm::vec2 G = glm::vec2(0.0f, -9.80665f);

void UpdatePhysics(SimulationSystem& sim, float deltaTime, bool useSpacePart)
{
    // Use reference to modify the actual particles
    std::vector<Particle>& particles = sim.GetParticles();
    int N = particles.size();

    // First update forces and positions for all particles
    for (int i = 0; i < N; i++)
    {
        // Reference to the current particle to modify it directly
        Particle& particleA = particles[i];

        // assume only grav. force
        particleA.force = particleA.mass * G;

        // Apply simple damping (air resistance)
        //particleA.force -= particleA.velocity * 0.1f;

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

        // Solve collision between particle and border
        SolveCollisionBorder(particleA, sim.GetBounds(), sim.GetParticleRadius());
        
        // Choose if using or not space partitioning 
        if (!useSpacePart) 
        {
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
    if (useSpacePart)
    {
        static SpatialGrid grid(
            sim.GetBounds().bottomLeft,
            sim.GetBounds().topRight,
            2.1f * 2.0f * sim.GetParticleRadius(), // Cell size (compute once)
            N
        );

        grid.Clear();

        // Insert all particles into the reused grid
        for (int i = 0; i < N; i++) {
            grid.InsertParticle(i, particles[i].position);
        }

        // Get collision pairs and resolve collisions
        std::vector<std::pair<int, int>> collisionPairs = grid.GetPotentialCollisionPairs();

        // Solve collision pairs
        for (const auto& pair : collisionPairs) 
            SolveCollisionParticle(particles[pair.first], particles[pair.second], sim.GetBounds(), sim.GetParticleRadius());
        
    }
}