#pragma once
#include "physics/SimulationSystem.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"

class VertexBufferLayout;

class ParticleRenderer
{
private:
    const SimulationSystem& m_Simulation;  // Reference to the simulation system
    const Shader& m_Shader;                // Reference to the shader program

    VertexArray* m_VertexArray;            // Vertex array for particles
    VertexBuffer* m_VertexBuffer;          // Vertex buffer for particle data

    // Initialize OpenGL buffers
    void InitBuffers();

public:
    // Constructor takes references to the simulation system and shader
    ParticleRenderer(const SimulationSystem& simulation, const Shader& shader);
    ~ParticleRenderer();

    // Update the buffer data from current particle state
    void UpdateBuffers();

    // Render the particles using the provided view matrix
    void Render(const SimulationSystem& simulation, const glm::mat4& viewMatrix);
};