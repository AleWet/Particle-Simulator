#pragma once

#include <vector>
#include <GL/glew.h>
#include "Renderer.h"

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		}
		ASSERT(false);
		return 0;
	}

	VertexBufferElement(unsigned int t, unsigned int c, bool n)
		: type(t), count(c), normalized(n) //this fixes an initialized variable problem, before the variables were un-initialized when the object was created and caused an issue with the push back
	{

	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0) { }

	template<typename T> //handle multiple types of data that are pushed as template information
	void Push(unsigned int count)
	{ 
		static_assert(false, "unsupported type");
	}
	
	template<>
	void Push<float>(unsigned int count)
	{
		//m_Elements.push_back({GL_FLOAT, count, false }); //Hardcoded for the moment
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		//m_Elements.push_back({GL_FLOAT, count, false }); //Hardcoded for the moment
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int  count)
	{
		//m_Elements.push_back({GL_FLOAT, count, false }); //Hardcoded for the moment
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE }); // not sure why this is normalized
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};