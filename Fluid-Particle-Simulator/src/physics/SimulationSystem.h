#pragma once

#include <vector>
#include "Particle.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class SimulationSystem
{
private:
    std::vector<Particle> m_Particles;
    glm::vec2 m_bottomLeft;  
    glm::vec2 m_topRight;    
    unsigned int m_ParticleRadius;

public:
    // bottomLeft is the bottom-left corner of the simulation rectangle and
    // topRight is the top-right corner of the simulation rectangle.
    // Initialize the size of a single particle 
    // Call this function once per simulation, calling it multiple times will delete previous simulation.
    // For the moment there are no visuals for bounds of the simulation
    SimulationSystem(const glm::vec2& bottomLeft, const glm::vec2& topRight, unsigned int particleRadius);
    ~SimulationSystem();

    // add new particle to particle vector, default mass is 1.0f. 
    void AddParticle(const glm::vec2& position, float mass = 1.0f);

    // add new particle grid in a rectangle formed between bottomLeft and topRight vectors.
    // The number of added particles is (rows * cols), the first particle is added at the top-left corner of the rectangle 
    // Make sure to not add particles on top of existing particles to avoid errors.
    // Spacing between particles is added according to the components of the input spacing vector.
    void AddParticleGrid(int rows, int cols, const glm::vec2& bottomLeft, const glm::vec2& topRight,
        const glm::vec2& spacing, float mass = 1.0f);

    const std::vector<Particle>& GetParticles() const { return m_Particles; } // THIS ONE IS JUST OT COPY 
    std::vector<Particle>& GetParticles() { return m_Particles; } // THIS ONE IS TO MODIFY THE VECTORIT

    const std::vector<glm::vec2> GetBounds() const
    {
        std::vector<glm::vec2> bounds(2);
        bounds[0] = m_bottomLeft;
        bounds[1] = m_topRight;
        return bounds;
    }
    
    // Create a view matrix for rendering the simulation
    // zoom: Zoom factor (1.0 = normal, >1.0 = zoom in, <1.0 = zoom out)
    // aspectRatio: Window width / height
    glm::mat4 GetViewMatrix(float zoom, float aspectRatio) const;
};