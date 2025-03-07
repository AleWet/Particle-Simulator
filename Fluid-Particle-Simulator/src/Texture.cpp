#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& path)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(1); 
	// load image for texture that needs to be flipped, openGL expects textures to
	// start from the bottom-left corner whereas in most of the standard image/files
	// you start from the top-left corner. 		  					 


	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height,&m_BPP,4); 
	// stbi_load takes the file path and populates the width, height, and BPP variables
	// with the image's dimensions and bits-per-pixel. The final parameter (4) specifies
	// the desired number of channels in the output image - in this case RGBA format.
	// Channels represent how color information is stored for each pixel: 
	// RGB + A (alpha, trasparency).


	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID)); 
	// for the moment it's hardcoded to 2D

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); 
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); 
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	// set texture parameters (i is for integer for some reason) 
	// texture minification and magnification will be discussed in 
	// a future episode of the series. These parameters have to be specified,
	// they are not defaulted and if you forget them you'll have a black screen

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));	
	// again, this will be specified in a future episode but the gist is that
	// you are specifing how the information is being setup inside the buffer that
	// is storing your texture data to then send it to the GPU

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	// unbind texture

	if (m_LocalBuffer) // if this buffer has data
	{
		stbi_image_free(m_LocalBuffer); // free local buffer (?????)
	}
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot /* optional, default = 0*/) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // texture ID locations are in a contiguous memory
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

}

void Texture::UnBind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
