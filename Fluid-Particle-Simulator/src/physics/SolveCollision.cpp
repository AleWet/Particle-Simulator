#include "SolveCollision.h"

void SolveCollisionBorder(Particle& particleA,
    const std::vector<glm::vec2> simBounds,
    unsigned int particleRadius)
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