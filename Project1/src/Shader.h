#pragma once
#include <string>
#include <unordered_map>

class Shader {
private:
	std::string m_FragmentFilePath;
	std::string m_VertexFilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& fragmentFilename, const std::string& vertexFilename);
	~Shader();

	void Bind() const;
	void Unbind() const;

	//Set uniforms
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
	std::string ParseShader(const std::string& filepath);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int GetUniformLocation(const std::string& name);
};