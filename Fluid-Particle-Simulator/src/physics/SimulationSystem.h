#pragma once

#include <vector>
#include "Particle.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Object to control the simulation
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
    float m_SimulationBorderOffSet;

public:
    // bottomLeft is the bottom-left corner of the simulation rectangle and
    // topRight is the top-right corner of the simulation rectangle.
    // Initialize the size of a single particle 
    // Call this function once per simulation, calling it multiple times will delete previous simulation.
    // For the moment there are no visuals for bounds of the simulation
    SimulationSystem(const glm::vec2& bottomLeft, const glm::vec2& topRight, unsigned int particleRadius, unsigned int windowWidth, float simulationBorderOffset);
    ~SimulationSystem();

    // Add new particle to particle vector, default mass is 1.0f. 
    void AddParticle(const glm::vec2& position, float mass = 1.0f);

    // Function used to create a grid of (rows * cols) particles, the particles will be 
    // automatically generated in the top-left corner of the simulation. By default the 
    // particles do not touch eachother when being spawned. Additionaly you 
    // can input a vec2 with the x and y spacing values for the particles. On top
    // of this the particles are separated by their radius regardless of the prev. input.
    // This is to avoid a bug that doesn't separate the particles
    void AddParticleGrid(int rows, int cols, glm::vec2 spacing, float mass = 1.0f);

    const std::vector<Particle>& GetParticles() const { return m_Particles; } // THIS ONE IS JUST OT COPY 
    std::vector<Particle>& GetParticles() { return m_Particles; } // THIS ONE IS TO MODIFY THE VECTORIT

    const std::vector<glm::vec2> GetBounds() const
    {
        std::vector<glm::vec2> bounds(2);
        bounds[0] = m_bottomLeft;
        bounds[1] = m_topRight;
        return bounds;
    }
    
    // Return projection matrix for rendering the simulation
    glm::mat4 GetProjMatrix() const;

    // Return a view matrix for the simulation
    glm::mat4 GetViewMatrix() const;

    float GetParticleRadius() const { return m_ParticleRadius; }

    // Convert a particle radius in screen pixels to simulation units
    // This helper function exists because OpenGL renders points as
    // rectangles and uses the side in pixels.
    float GetParticleRenderSize() const;

    // Return simulation zoom
    float GetZoom() const { return m_Zoom; }

    // Set the zoom level
    void SetZoom(float zoom) { m_Zoom = zoom; }

    // Return simulation heigth
    float GetSimHeight() const { return m_SimHeight; }

    // Return simulation width
    float GetSimWidth() const { return m_SimWidth; }

    // Return window width
    void SetWindowWidth(unsigned int width) { m_WindowWidth = width; }

};