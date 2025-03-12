#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
	size_t m_Size;
public:
	VertexBuffer(const void* data, unsigned int size, unsigned int usage);
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;
	void Resize(size_t newSize);
	size_t GetSize() const { return m_Size; }
};