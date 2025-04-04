#pragma once

#include <vector>
#include "Particle.h"
#include "glm/gtc/matrix_transform.hpp"
#include "SpatialGrid.h" 

struct Bounds {
    Vec2 bottomLeft;
    Vec2 topRight;
};

// Object to control the simulation
class SimulationSystem
{
private:
    std::vector<Particle> m_Particles;     
    Bounds m_Bounds;
    float m_ParticleRadius;
    float m_Zoom;
    float m_SimHeight;
    float m_SimWidth;
    unsigned int m_WindowWidth;
    bool m_UseSpatialGrid = true;
    SpatialGrid* m_SpatialGrid = nullptr;

    struct ParticleStream {
        bool isActive = false;
        Vec2 startPos;
        Vec2 velocity;
        int total = 0;
        int spawned = 0;
        float spawnInterval = 0.0f;
        float timer = 0.0f;
        float mass = 1.0f;  
    };

    std::vector<ParticleStream> m_Streams;

public:
    // bottomLeft is the bottom-left corner of the simulation rectangle and
    // topRight is the top-right corner of the simulation rectangle.
    // Initialize the size of a single particle. The simulation is always centered.
    // Call this function once per simulation, calling it multiple times will delete previous simulation.
    // For the moment there are no visuals for bounds of the simulation
    SimulationSystem(const Vec2& bottomLeft, const Vec2& topRight, float particleRadius, unsigned int windowWidth);
    ~SimulationSystem();

    // Add new particle to particle vector, default mass is 1.0f. 
    void AddParticle(const Vec2& position, const Vec2& velocity, float mass = 1.0f);

    // Function used to create a grid of (rows * cols) particles, the particles will be 
    // automatically generated in the top-left corner of the simulation. By default the 
    // particles do not touch eachother when being spawned. Additionaly you 
    // can input a vec2 with the x and y spacing values for the particles. On top
    // of this the particles are separated by their radius regardless of the prev. input.
    // This is to avoid a bug that doesn't separate the particles
    void AddParticleGrid(int rows, int cols, Vec2 spacing, bool withInitialVelocity, float mass = 1.0f);

    void AddParticleStream(int totalParticles, float spawnRate, const Vec2& velocity,
        float mass, const Vec2& initialOffset);

    // Replace StartParticleStream with AddParticleStream
    // Update the UpdateStream method
    void UpdateStreams(float deltaTime);

    // Method to clear all streams
    void ClearStreams() { m_Streams.clear(); }

    // Method to get active stream count
    size_t GetActiveStreamCount() const { return m_Streams.size(); }

    const std::vector<Particle>& GetParticles() const { return m_Particles; } // THIS ONE IS JUST OT COPY 
    std::vector<Particle>& GetParticles() { return m_Particles; } // THIS ONE IS TO MODIFY THE VECTORIT

    const Bounds& GetBounds() const { return m_Bounds; }
    
    // Return projection matrix for rendering the simulation
    glm::mat4 GetProjMatrix() const;

    // Return a view matrix for the simulation
    glm::mat4 GetViewMatrix() const;

    // Return particle radius
    float GetParticleRadius() const { return m_ParticleRadius; }

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

    bool IsUsingSpatialGrid() const { return m_UseSpatialGrid; }
    void SetUseSpatialGrid(bool use) { m_UseSpatialGrid = use; }

    // Initialize the spatial grid
    void InitSpatialGrid();

    // Get the spatial grid
    SpatialGrid* GetSpatialGrid() { return m_SpatialGrid; }
};