#include "ParticleRenderer.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include <iostream>

ParticleRenderer::ParticleRenderer(const SimulationSystem& simulation, const Shader& shader)
    : m_Simulation(simulation), m_Shader(shader), m_VertexArray(nullptr),
    m_VertexBuffer(nullptr), m_InstanceBuffer(nullptr), m_IndexBuffer(nullptr)
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

    if (m_InstanceBuffer) {
        delete m_InstanceBuffer;
        m_InstanceBuffer = nullptr;
    }

    if (m_IndexBuffer) {
        delete m_IndexBuffer;
        m_IndexBuffer = nullptr;
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

    // Create a quad (2 triangles) that will be instanced for each particle
    // These vertices define a quad from (-1,-1) to (1,1)
    float quadVertices[] = {
        // positions      // texture coords
        -1.0f, -1.0f,     0.0f, 0.0f,  // bottom left
         1.0f, -1.0f,     1.0f, 0.0f,  // bottom right
         1.0f,  1.0f,     1.0f, 1.0f,  // top right
        -1.0f,  1.0f,     0.0f, 1.0f   // top left
    };

    // Create indices for the quad (2 triangles)
    unsigned int quadIndices[] = {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };

    // Create vertex buffer for the quad
    m_VertexBuffer = new VertexBuffer(quadVertices, sizeof(quadVertices), GL_STREAM_DRAW);

    // Create index buffer
    m_IndexBuffer = new IndexBuffer(quadIndices, 6);

    // Set up vertex buffer layout
    VertexBufferLayout quadLayout;
    quadLayout.Push<float>(2);  // Position (vec2)
    quadLayout.Push<float>(2);  // Texture coordinates (vec2)

    // Link the vertex buffer to the vertex array
    m_VertexArray->AddBuffer(*m_VertexBuffer, quadLayout);

    // Bind the index buffer to the vertex array
    m_VertexArray->Bind();
    m_IndexBuffer->Bind();

    // Allocate based on current particle count
    const size_t initialBufferSize = sizeof(ParticleInstance) * m_Simulation.GetParticles().size();
    m_InstanceBuffer = new VertexBuffer(nullptr, initialBufferSize, GL_STREAM_DRAW);

    // Set up instance buffer layout
    VertexBufferLayout instanceLayout;
    instanceLayout.Push<float>(2);  // Position (vec2)
    instanceLayout.Push<float>(4);  // Color (vec4)
    instanceLayout.Push<float>(1);  // Size (float)

    // Configure the instance buffer attributes
    m_VertexArray->Bind();
    m_InstanceBuffer->Bind();

    // The instance data needs to be linked to the VAO with a divisor
    // This tells OpenGL that these attributes advance once per instance, not per vertex
    GLCall(glEnableVertexAttribArray(2)); // Start after the quad attributes (0,1)
    GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleInstance), (void*)0));
    GLCall(glVertexAttribDivisor(2, 1)); // Position (advance one instance at a time)

    GLCall(glEnableVertexAttribArray(3));
    GLCall(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstance), (void*)(2 * sizeof(float))));
    GLCall(glVertexAttribDivisor(3, 1)); // Velocity (advance one instance at a time)

    GLCall(glEnableVertexAttribArray(4));
    GLCall(glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleInstance), (void*)(4 * sizeof(float))));
    GLCall(glVertexAttribDivisor(4, 1)); // Size (advance one instance at a time)

    // Unbind everything
    m_VertexArray->UnBind();
    m_VertexBuffer->UnBind();
    m_InstanceBuffer->UnBind();
    m_IndexBuffer->UnBind();
}


void ParticleRenderer::UpdateBuffers()
{
    // Get particles from simulation
    const std::vector<Particle>& particles = m_Simulation.GetParticles();
    const size_t particleCount = particles.size();

    if (particleCount == 0) {
        return;
    }

    // Resize only if needed, preserving capacity
    if (m_InstanceData.size() < particleCount) {
        m_InstanceData.resize(particleCount);
    }

    // Update instance data with particle positions and velocities
    float particleRadius = m_Simulation.GetParticleRadius();
    for (size_t i = 0; i < particleCount; i++) {
        const Particle& particle = particles[i];
        m_InstanceData[i].position = particle.position;
        m_InstanceData[i].velocity = particle.velocity;
        m_InstanceData[i].size = particleRadius;
    }


    // Update buffer
    m_InstanceBuffer->Bind();
    size_t dataSize = sizeof(ParticleInstance) * particleCount;

    // Only reallocate if buffer is too small
    if (dataSize > m_InstanceBuffer->GetSize()) {
        // Allocate with some growth factor to avoid frequent resizing
        size_t newSize = dataSize * 2;
        glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_STREAM_DRAW);
        m_InstanceBuffer->Resize(newSize);
    }

    // Update the data
    glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_InstanceData.data());
    m_InstanceBuffer->UnBind();
}

void ParticleRenderer::Render()
{
    // No particles to render
    if (m_Simulation.GetParticles().empty())
        return;

    // Create MVP for particles
    glm::mat4 particleMVP = m_Simulation.GetProjMatrix() * m_Simulation.GetViewMatrix();

    // Bind shader and set uniforms
    m_Shader.Bind();
    m_Shader.setUniformMat4f("u_MVP", particleMVP);

    // Bind vertex array and index buffer
    m_VertexArray->Bind();
    m_IndexBuffer->Bind();

    // Draw instanced quads
    GLCall(glDrawElementsInstanced(
        GL_TRIANGLES,
        6,                                                       // 6 indices per quad (2 triangles)
        GL_UNSIGNED_INT,
        0,
        static_cast<GLsizei>(m_Simulation.GetParticles().size()) // Number of instances
    ));

    // Unbind everything
    m_VertexArray->UnBind();
    m_IndexBuffer->UnBind();
    m_Shader.UnBind();
}