#include "SolveCollision.h"

// Value between 0 (inelastic) and 1 (perfectly elastic)
const float BOUNCINESS = 1.0f;

void SolveCollisionBorder(Particle& particleA,
    const Bounds bounds,
    float particleRadius)
{
    // Extract boundary coordinates
    const glm::vec2& bottomLeft = bounds.bottomLeft;
    const glm::vec2& topRight = bounds.topRight;

    // Calculate particle radius in simulation units
    float radius = static_cast<float>(particleRadius);

    // Store the original velocity for calculating reflection
    glm::vec2 originalVelocity = particleA.velocity;
    bool collided = false;

    // Horizontal bounds check
    if (particleA.position.x - radius < bottomLeft.x) {
        particleA.position.x = bottomLeft.x + radius;
        particleA.velocity.x = -particleA.velocity.x;
        collided = true;
    }
    else if (particleA.position.x + radius > topRight.x) {
        particleA.position.x = topRight.x - radius;
        particleA.velocity.x = -particleA.velocity.x;
        collided = true;
    }

    // Vertical bounds check
    if (particleA.position.y - radius < bottomLeft.y) {
        particleA.position.y = bottomLeft.y + radius;
        particleA.velocity.y = -particleA.velocity.y;
        collided = true;
    }
    else if (particleA.position.y + radius > topRight.y) {
        particleA.position.y = topRight.y - radius;
        particleA.velocity.y = -particleA.velocity.y;
        collided = true;
    }

    // Add energy loss during collision (coefficient of restitution)
    if (collided) {
        
        //particleA.velocity *= 0.95f;
    }
}

void SolveCollisionParticle(Particle& particleA, Particle& particleB,
    const Bounds bounds, float particleRadius)
{
    // Manual position delta and distance calculation
    const float dx = particleA.position.x - particleB.position.x;
    const float dy = particleA.position.y - particleB.position.y;
    const float distanceSquared = dx * dx + dy * dy;
    const float minDistanceSquared = 4.0f * particleRadius * particleRadius;

    if (distanceSquared < minDistanceSquared)
    {
        const float distance = sqrt(distanceSquared);
        if (distance < 1e-5f) return;

        // Manually normalize collision normal (avoid glm::vec2 division)
        const float invDistance = 1.0f / distance;
        const float nx = dx * invDistance;
        const float ny = dy * invDistance;

        // Position correction
        const float overlap = 2.0f * particleRadius - distance;
        const float totalMass = particleA.mass + particleB.mass;
        const float ratioA = particleB.mass / totalMass;
        const float ratioB = particleA.mass / totalMass;

        particleA.position.x += nx * overlap * ratioA;
        particleA.position.y += ny * overlap * ratioA;
        particleB.position.x -= nx * overlap * ratioB;
        particleB.position.y -= ny * overlap * ratioB;

        // Velocity resolution
        const float vx = particleA.velocity.x - particleB.velocity.x;
        const float vy = particleA.velocity.y - particleB.velocity.y;
        const float velocityAlongNormal = vx * nx + vy * ny;

        if (velocityAlongNormal < 0)
        {
            const float restitution = BOUNCINESS;
            const float impulseScalar = -(1.0f + restitution) * velocityAlongNormal;
            const float impulse = impulseScalar / (1.0f / particleA.mass + 1.0f / particleB.mass);

            particleA.velocity.x += impulse * nx / particleA.mass;
            particleA.velocity.y += impulse * ny / particleA.mass;
            particleB.velocity.x -= impulse * nx / particleB.mass;
            particleB.velocity.y -= impulse * ny / particleB.mass;

            // Temperature update (optional)
            const float collisionIntensity = sqrt(impulse * impulse) * 0.01f;
            particleA.temperature = std::min(100.0f, particleA.temperature + collisionIntensity);
            particleB.temperature = std::min(100.0f, particleB.temperature + collisionIntensity);
        }
    }
}