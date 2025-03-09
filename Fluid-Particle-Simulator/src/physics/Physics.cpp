#include "SimulationSystem.h"
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
        Particle& curParticle = particles[i];

        // assume only grav. force
        curParticle.force = curParticle.mass * G;

        // Apply simple damping (air resistance)
        //curParticle.force -= curParticle.velocity * 0.1f;

        // Update velocity based on force
        curParticle.velocity += (curParticle.force / curParticle.mass) * deltaTime;

        // Update position
        curParticle.position += curParticle.velocity * deltaTime;

        // Simple boundary collision handling
        const std::vector<glm::vec2> bounds = sim.GetBounds();
        const glm::vec2& bottomLeft = bounds[0];
        const glm::vec2& topRight = bounds[1];

        // Horizontal bounds check
        if (curParticle.position.x < bottomLeft.x) {
            curParticle.position.x = bottomLeft.x;
            curParticle.velocity.x *= -1.0f;  // 100% elastic
        }
        else if (curParticle.position.x > topRight.x) {
            curParticle.position.x = topRight.x;
            curParticle.velocity.x *= -1.0f;  // 100% elastic
        }

        // Vertical bounds check
        if (curParticle.position.y < bottomLeft.y) {
            curParticle.position.y = bottomLeft.y;
            curParticle.velocity.y *= -1.0f;  // 100% elastic
        }
        else if (curParticle.position.y > topRight.y) {
            curParticle.position.y = topRight.y;
            curParticle.velocity.y *= -1.0f;  // 100% elastic
        }

        // Add a small amount of temperature for fast-moving particles kinda like friction 
        float speed = glm::length(curParticle.velocity);
        if (speed > 5.0f) {
            curParticle.temperature = std::min(100.0f, curParticle.temperature + 0.1f);
        }
        else {
            // Cool down
            curParticle.temperature = std::max(20.0f, curParticle.temperature - 0.05f);
        }
    }
}