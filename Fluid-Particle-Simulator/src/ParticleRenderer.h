#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "physics/SimulationSystem.h"

// Structure for the particle instance data that will be sent to the GPU
struct ParticleInstance {
    glm::vec2 position;  // Particle position
    glm::vec4 color;     // Particle color (RGBA)
    float size;          // Particle size
};

class ParticleRenderer {
private:
    const SimulationSystem& m_Simulation;
    const Shader& m_Shader;
    VertexArray* m_VertexArray;
    VertexBuffer* m_VertexBuffer;    // For the quad vertices
    VertexBuffer* m_InstanceBuffer;  // For the particle instance data
    IndexBuffer* m_IndexBuffer;      // For the quad indices
    std::vector<ParticleInstance> m_InstanceData; // Optimize memory allocation

public:
    ParticleRenderer(const SimulationSystem& simulation, const Shader& shader);
    ~ParticleRenderer();

    void InitBuffers();
    void UpdateInstanceDataColorVelocity(std::vector<ParticleInstance>& data, const std::vector<Particle>& particles);
    void UpdateBuffers();
    void Render();
};