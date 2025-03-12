#include "SolveCollision.h"

// Value between 0 (inelastic) and 1 (perfectly elastic)
const float BOUNCINESS = 1.0f;

void SolveCollisionBorder(Particle& particleA,
    const std::vector<glm::vec2> simBounds,
    float particleRadius)
{
    // Extract boundary coordinates
    const glm::vec2& bottomLeft = simBounds[0];
    const glm::vec2& topRight = simBounds[1];

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
    const std::vector<glm::vec2> simBounds,
    float particleRadius)
{
    // Calculate vector between particle centers
    glm::vec2 positionDelta = particleA.position - particleB.position;

    // Calculate squared distance between particles (optimization to avoid square root)
    float distanceSquared = glm::dot(positionDelta, positionDelta);

    // Calculate squared minimum distance before particles collide
    float minDistanceSquared = 4.0f * particleRadius * particleRadius;

    // Check if particles are overlapping
    if (distanceSquared < minDistanceSquared)
    {
        // Calculate actual distance between particle centers
        float distance = sqrt(distanceSquared);

        // Normalize the position delta vector
        glm::vec2 collisionNormal = positionDelta / distance;

        // Prevent division by zero
        if (distance < 1e-5f) return;

        // Move particles apart with exact separation
        const float separation = 2.0f * particleRadius;
        glm::vec2 correction = (separation - distance) * 0.5f * collisionNormal;
        particleA.position += correction;
        particleB.position -= correction;

        // Calculate overlap amount
        float overlap = 2.0f * particleRadius - distance;

        // Resolve position overlap by moving particles apart proportional to their masses
        float totalMass = particleA.mass + particleB.mass;
        float ratioA = particleB.mass / totalMass;
        float ratioB = particleA.mass / totalMass;

        // Move particles apart to resolve overlap
        particleA.position += collisionNormal * overlap * ratioA;
        particleB.position -= collisionNormal * overlap * ratioB;

        // Calculate relative velocity along collision normal
        glm::vec2 relativeVelocity = particleA.velocity - particleB.velocity;
        float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

        // Only resolve if particles are moving toward each other
        if (velocityAlongNormal < 0)
        {
            // Calculate coefficient of restitution
            float restitution = BOUNCINESS;

            // Calculate impulse scalar
            float impulseScalar = -(1.0f + restitution) * velocityAlongNormal;
            impulseScalar /= (1.0f / particleA.mass) + (1.0f / particleB.mass);

            // Apply impulse
            glm::vec2 impulse = impulseScalar * collisionNormal;
            particleA.velocity += impulse / particleA.mass;
            particleB.velocity -= impulse / particleB.mass;

            // Add temperature increase based on collision intensity
            float collisionIntensity = glm::length(impulse) * 0.01f;
            particleA.temperature = std::min(100.0f, particleA.temperature + collisionIntensity);
            particleB.temperature = std::min(100.0f, particleB.temperature + collisionIntensity);
        }
    }
}