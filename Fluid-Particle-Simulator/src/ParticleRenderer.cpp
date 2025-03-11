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

        // Color based on velocity, not temperature (blue -> green -> red)
        float normalizedV = (std::min(glm::length(particle.velocity), 100.0f)) / 100.0f;
        normalizedV = glm::clamp(normalizedV, 0.0f, 1.0f);

        // Use a continuous RGB gradient from blue -> cyan -> green -> yellow -> red
        // (0,0,1) -> (0,1,1) -> (0,1,0) -> (1,1,0) -> (1,0,0)
        glm::vec3 color;

        if (normalizedV < 0.25f) {
            // Blue to Cyan (0,0,1) -> (0,1,1)
            float t = normalizedV / 0.25f;
            color = glm::vec3(0.0f, t, 1.0f);
        }
        else if (normalizedV < 0.5f) {
            // Cyan to Green (0,1,1) -> (0,1,0)
            float t = (normalizedV - 0.25f) / 0.25f;
            color = glm::vec3(0.0f, 1.0f, 1.0f - t);
        }
        else if (normalizedV < 0.75f) {
            // Green to Yellow (0,1,0) -> (1,1,0)
            float t = (normalizedV - 0.5f) / 0.25f;
            color = glm::vec3(t, 1.0f, 0.0f);
        }
        else {
            // Yellow to Red (1,1,0) -> (1,0,0)
            float t = (normalizedV - 0.75f) / 0.25f;
            color = glm::vec3(1.0f, 1.0f - t, 0.0f);
        }

        vertexData[i].color = glm::vec4(color, 1.0f);
        
        vertexData[i].size = m_Simulation.GetParticleRenderSize();
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

void ParticleRenderer::Render()
{
    // No particles to render
    if (m_Simulation.GetParticles().empty()) {
        return;
    }

    // Create MVP for particles
    glm::mat4 particleMVP = m_Simulation.GetProjMatrix() * m_Simulation.GetViewMatrix();
    
    // Bind shader and set uniforms
    m_Shader.Bind();
    // For the moment the viewMatrix is the MVP matrix, the camera can't move
    m_Shader.setUniformMat4f("u_MVP", particleMVP);

    // Set uniform to scale up particle radius to expected size
    // Account for the fact gl_PointSize expects the diameter in pixels, not the radius ==> double it
    float pointSizeScale = 2.0f; // Double the radius to get diameter
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