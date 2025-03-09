#include "ParticleRenderer.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include <iostream>

// Structure for the particle vertex data that will be sent to the GPU
struct ParticleVertex {
    glm::vec2 position;  // Particle position
    glm::vec4 color;     // Particle color (RGBA)
    float size;          // Particle size
};

ParticleRenderer::ParticleRenderer(const SimulationSystem& simulation, const Shader& shader)
    : m_Simulation(simulation), m_Shader(shader), m_VertexArray(nullptr), m_VertexBuffer(nullptr)
{
    // Initialize buffers
    InitBuffers();
}

ParticleRenderer::~ParticleRenderer()
{
    // Clean up resources
    if (m_VertexBuffer) {
        delete m_VertexBuffer;
        m_VertexBuffer = nullptr;
    }

    if (m_VertexArray) {
        delete m_VertexArray;
        m_VertexArray = nullptr;
    }
}

void ParticleRenderer::InitBuffers()
{
    // Create a new vertex array
    m_VertexArray = new VertexArray();

    // Create an empty vertex buffer initially (update in UpdateBuffers)
    // Start with a arbitrary size for the buffer
    const size_t initialBufferSize = sizeof(ParticleVertex) * 1000; // Allocate for 1000 particles initially
    m_VertexBuffer = new VertexBuffer(nullptr, initialBufferSize);

    // Set up the vertex buffer layout
    VertexBufferLayout layout;
    layout.Push<float>(2);  // Position (vec2)
    layout.Push<float>(4);  // Color (vec4)
    layout.Push<float>(1);  // Size (float)

    // Link the vertex buffer to the vertex array with the defined layout
    m_VertexArray->AddBuffer(*m_VertexBuffer, layout);
}

void ParticleRenderer::UpdateBuffers()
{
    // Get the particles from the simulation system
    const std::vector<Particle>& particles = m_Simulation.GetParticles();
    const size_t particleCount = particles.size();

    if (particleCount == 0) {
        return; 
    }

    // Create a vector of ParticleVertex for the updated data
    std::vector<ParticleVertex> vertexData(particleCount);

    // Update vertex data for each particle
    for (size_t i = 0; i < particleCount; i++) {
        const Particle& particle = particles[i];

        // Set position directly from particle
        vertexData[i].position = particle.position;

        // Color based on particle temperature (blue -> white -> red)
        // Normalize temperature to 0-1 range for color calculation
        float normalizedTemp = (particle.temperature - 20.0f) / 80.0f; // Assuming temp range 20-100
        normalizedTemp = glm::clamp(normalizedTemp, 0.0f, 1.0f);

        if (normalizedTemp < 0.5f) {
            // Blue to white gradient (cold to medium)
            float t = normalizedTemp * 2.0f;
            vertexData[i].color = glm::vec4(t, t, 1.0f, 0.8f);
        }
        else {
            // White to red gradient (medium to hot)
            float t = (normalizedTemp - 0.5f) * 2.0f;
            vertexData[i].color = glm::vec4(1.0f, 1.0f - t, 1.0f - t, 0.8f);
        }
        
        vertexData[i].size = m_Simulation.GetParticleRadius();
    }

    // Delete old buffer and create a new one with updated data
    if (m_VertexBuffer) {
        m_VertexBuffer->UnBind();
        delete m_VertexBuffer;
    }
    
    // Create a new vertex buffer with the updated data
    m_VertexBuffer = new VertexBuffer(vertexData.data(), sizeof(ParticleVertex) * particleCount);

    // Re-attach the new buffer to the vertex array
    VertexBufferLayout layout;
    layout.Push<float>(2);  // Position (vec2)
    layout.Push<float>(4);  // Color (vec4)
    layout.Push<float>(1);  // Size (float)

    m_VertexArray->AddBuffer(*m_VertexBuffer, layout);
}

void ParticleRenderer::Render(const SimulationSystem& sim, const glm::mat4& viewMatrix)
{
    // No particles to render
    if (m_Simulation.GetParticles().empty()) {
        return;
    }

    // aspect ratio
    // simulation borderd offset

    // Bind shader and set uniforms
    m_Shader.Bind();
    // For the moment the viewMatrix is the MVP matrix, the camera can't move
    m_Shader.setUniformMat4f("u_MVP", viewMatrix); 

    // Set uniform to scale up particle radius to expected size
    // Account for the fact gl_PointSize expects the diameter, not the radius ==> double it
    float pointSizeScale = sim.GetParticleRenderSize(sim.GetParticleRadius()) * 2.0f;
    m_Shader.setUniform1f("u_PointSizeScale", pointSizeScale);

    // Bind vertex array
    m_VertexArray->Bind();

    // Enable point size (ensure this is enabled)
    GLCall(glEnable(GL_PROGRAM_POINT_SIZE));

    // Draw the particles as points
    GLCall(glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_Simulation.GetParticles().size())));

    // Unbind everything
    m_VertexArray->UnBind();
    m_Shader.UnBind();
}