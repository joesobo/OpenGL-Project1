#include "Shader.h"
#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& fragmentFilepath, const std::string& vertexFilepath)
	: m_FragmentFilePath(fragmentFilepath), m_VertexFilePath(vertexFilepath), m_RendererID(0)
{
	std::string vertexSource = ParseShader(vertexFilepath);
	std::string fragmentSource = ParseShader(fragmentFilepath);

	m_RendererID = CreateShader(vertexSource, fragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program;
	GLCall(program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)_malloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

std::string Shader::ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

	std::string line;
	std::stringstream ss;
	while (getline(stream, line)) {
		ss << line << '\n';
	}

	return ss.str();
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1) {
		std::cout << "Warning : uniform '" << name << "' doesn't exist!" << std::endl;
	}

	m_UniformLocationCache[name] = location;
	return location;
}
