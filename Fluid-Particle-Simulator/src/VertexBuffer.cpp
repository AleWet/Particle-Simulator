#include "VertexBuffer.h"
#include "Renderer.h"
#include <iostream>

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
    GLCall(glGenBuffers(1, &m_RendererID));
    m_Size = size;

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    
    if (size == 0) 
    {
        std::cout << "Warning: Creating an empty buffer with size 0" << std::endl;
        // Minimal valid buffer
        GLCall(glBufferData(GL_ARRAY_BUFFER, 1, nullptr, GL_STATIC_DRAW));
    }
    else 
    {
        // Allocate full size even if data is nullptr
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW)); // Use DYNAMIC_DRAW
    }

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::UnBind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::Resize(size_t newSize) {
    m_Size = newSize;
    Bind();
    glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW); 
    UnBind();
}

