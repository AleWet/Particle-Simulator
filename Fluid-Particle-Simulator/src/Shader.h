#pragma once
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};
	
class Shader
{
private:
	std::string m_FilePath; 
	unsigned int m_RendererID;
	mutable  std::unordered_map<std::string, int> m_UniformLocationCache; //cache system

public:
	Shader(const std::string& filepath); // since I only have one shader file I need one path to it
	~Shader();
	
	void Bind() const;	// change from standar openGL API from useProgram to Bind to be consistent 
	void UnBind() const; 

	//set uniforms
	void setUniform1i(const std::string& name, int value) const;
	void setUniform1f(const std::string& name, float value) const;
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) const;
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix) const;
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	int GetUniformLocation(const std::string& name) const;
};

