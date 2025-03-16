#include "physics.h"
#include "SpatialGrid.h"
#include "Vec2.h"  

const Vec2 G(0.0f, -20.80665f);
const float AIR_RESISTANCE = 0.0f;

void UpdatePhysics(SimulationSystem& sim, float deltaTime, bool useSpacePart)
{
    std::vector<Particle>& particles = sim.GetParticles();
    const int N = particles.size();

    for (int i = 0; i < N; i++)
    {
        Particle& particleA = particles[i];

        // Force calculation
        particleA.force.x = particleA.mass * G.x;
        particleA.force.y = particleA.mass * G.y;

        // Air resistance
        particleA.force.x -= particleA.velocity.x * AIR_RESISTANCE;
        particleA.force.y -= particleA.velocity.y * AIR_RESISTANCE;

        // Velocity integration
        particleA.velocity.x += (particleA.force.x / particleA.mass) * deltaTime;
        particleA.velocity.y += (particleA.force.y / particleA.mass) * deltaTime;

        // Position integration
        particleA.position.x += particleA.velocity.x * deltaTime;
        particleA.position.y += particleA.velocity.y * deltaTime;

        // Temperature calculation
        const float speed = particleA.velocity.length();
        if (speed > 5.0f) 
        {
            particleA.temperature = std::min(100.0f, particleA.temperature + 0.1f);
        }
        else 
        {
            particleA.temperature = std::max(20.0f, particleA.temperature - 0.05f);
        }

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
        std::vector<std::pair<int, int>> collisionPairs = grid.GetPotentialCollisionPairs(
                                                                sim.GetParticles(),
                                                                2 * sim.GetParticleRadius());

        // Solve collision pairs
        for (const auto& pair : collisionPairs) 
            SolveCollisionParticle(particles[pair.first], particles[pair.second], sim.GetBounds(), sim.GetParticleRadius());
        
    }
    sim.UpdateStreams(deltaTime);
}