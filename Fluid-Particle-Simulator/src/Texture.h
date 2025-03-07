#pragma once
#include "Renderer.h"


class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP; //bits per pixel
public:
	Texture(const std::string& filepath);
	~Texture();

	void Bind(unsigned slot = 0) const; //optional parameter to bind a texture to a different slot
	void UnBind() const;

	inline int getWidth() const { return m_Width; }
	inline int getHeight() const { return m_Height; }


};