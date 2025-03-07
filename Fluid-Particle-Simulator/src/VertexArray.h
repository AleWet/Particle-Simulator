#pragma once

#include "VertexBuffer.h"

class VertexBufferLayout; // need this to avoid infinite loop with include?

class VertexArray
{
private:
	unsigned int m_RendererID;
		
public:
	VertexArray();
	~VertexArray();


	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void UnBind() const;
};