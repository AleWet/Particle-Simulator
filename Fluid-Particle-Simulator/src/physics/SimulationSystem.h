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
    float m_Zoom;
    float m_SimHeight;
    float m_SimWidth;
    unsigned int m_WindowWidth;

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
    // aspectRatio: Window width / Window height.
    // For the moment this is the only transformation needed
    glm::mat4 GetViewMatrix(float aspectRatio, float simulationBorderOffset) const;

    float GetParticleRadius() const { return m_ParticleRadius; }

    // Convert a distance in screen pixels to simulation units
    // pixelDistance: Distance in screen pixels
    float PixelToSimulationDistance(float pixelDistance) const;

    float GetZoom() const
    {
        return m_Zoom;
    }

    // Set the zoom level
    void SetZoom(float zoom)
    {
        m_Zoom = zoom;
    }

    float GetSimHeight() const { return m_SimHeight; }

    void SetWindowWidth(unsigned int width) {
        m_WindowWidth = width;
    }

    float GetSimWidth() const { return m_SimWidth; }
};