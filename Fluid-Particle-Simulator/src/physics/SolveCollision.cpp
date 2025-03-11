#include "SolveCollision.h"

void SolveCollisionBorder(Particle& particleA, 
                        const std::vector<glm::vec2> simBounds, 
                        unsigned int particleRadius)
{
    // Simple boundary collision handling
    const glm::vec2& bottomLeft = simBounds[0];
    const glm::vec2& topRight = simBounds[1];

    // Horizontal bounds check, the + 1 is just for aesthetics
    if (particleA.position.x < bottomLeft.x + particleRadius + 1) {
        particleA.position.x = bottomLeft.x + particleRadius + 1;
        particleA.velocity.x *= -1.0f;  // 100% elastic
    }
    else if (particleA.position.x > topRight.x - particleRadius) {
        particleA.position.x = topRight.x - particleRadius;
        particleA.velocity.x *= -1.0f;  // 100% elastic
    }

    // Vertical bounds check
    if (particleA.position.y < bottomLeft.y + particleRadius + 1) {
        particleA.position.y = bottomLeft.y + particleRadius + 1;
        particleA.velocity.y *= -1.0f;  // 100% elastic
    }
    else if (particleA.position.y > topRight.y - particleRadius) {
        particleA.position.y = topRight.y - particleRadius;
        particleA.velocity.y *= -1.0f;  // 100% elastic
    }
}

void SolveCollisionParticle(Particle& particleA, Particle& particleB, 
                            const std::vector<glm::vec2> simBounds, 
                            unsigned int particleRadius)
{

}