#include "VertexBuffer.h"
#include "Renderer.h"
#include <iostream>

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    GLCall(glGenBuffers(1, &m_rendererID));

    // Add validation
    if (data == nullptr || size == 0)
    {
        std::cout << "Warning: Creating an empty buffer" << std::endl;
        // Initialize with empty buffer
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
        GLCall(glBufferData(GL_ARRAY_BUFFER, 1, nullptr, GL_STATIC_DRAW)); // Minimal valid buffer
    }
    else
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    }

    // Unbind the buffer to avoid state pollution (Gammò?)
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_rendererID));
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
}

void VertexBuffer::UnBind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}