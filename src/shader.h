#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader
{
public:
	Shader() = delete;

	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geometryShaderPath = "")
	{
		const auto& [vertexSource, fragmentSource, geometrySource] = ParseShader(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
		m_rendererID = CreateShader(vertexSource, fragmentSource, geometrySource);

#ifdef _DEBUG
		std::cout << "successfully create: " << "\n" << vertexShaderPath << "\n" <<
			fragmentShaderPath << "\n" << geometryShaderPath << "\n";
#endif 

	}

	~Shader()
	{
		glDeleteProgram(m_rendererID);
	}

	void Bind() const
	{
		glUseProgram(m_rendererID);
	}

	void Unbind() const
	{
		glUseProgram(0);
	}

	unsigned int GetID() const
	{
		return m_rendererID;
	}

	void SetVec3(const std::string& _name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, _name.c_str());
#ifdef _DEBUG
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << _name << "' not found or shader program not linked.\n";
		}
#endif
		glUniform3fv(location, 1, &value[0]);
	}

	void SetVec3(const std::string& _name, float _x, float _y, float _z)
	{
		glm::vec3 value(_x, _y, _z);
		GLint location = glGetUniformLocation(m_rendererID, _name.c_str());
#ifdef _DEBUG
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << _name << "' not found or shader program not linked.\n";
		}
#endif
		glUniform3fv(location, 1, &value[0]);
	}

	void SetVec2(const std::string& _name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, _name.c_str());
#ifdef _DEBUG
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << _name << "' not found or shader program not linked.\n";
		}
#endif
		glUniform2fv(location, 1, &value[0]);
	}

	void SetMat4(const std::string& _name, const glm::mat4& _mat)
	{
		GLint location = glGetUniformLocation(m_rendererID, _name.c_str());
#ifdef _DEBUG
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << _name << "' not found or shader program not linked.\n";
		}
#endif
		glUniformMatrix4fv(location, 1, GL_FALSE, &_mat[0][0]);
	}

	void SetFloat(const std::string& _name, float _value)
	{
		GLint location = glGetUniformLocation(m_rendererID, _name.c_str());
#ifdef _DEBUG
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << _name << "' not found or shader program not linked.\n";
		}
#endif
		glUniform1f(location, _value);
	}

	void SetInt(const std::string& _name, int _value) const
	{
		GLint location = glGetUniformLocation(m_rendererID, _name.c_str());
#ifdef _DEBUG
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << _name << "' not found or shader program not linked.\n";
		}
#endif
		glUniform1i(location, _value);
	}

	void BindUniformBlock(const std::string& uniformBlockName, unsigned int bindingPoint)
	{
		unsigned int blockIndex = glGetUniformBlockIndex(m_rendererID, uniformBlockName.c_str());
#ifdef _DEBUG
		if (blockIndex == GL_INVALID_INDEX) {
			std::cerr << "Warning: Uniform block \"" << uniformBlockName << "\" not found in shader.\n";
		}
#endif
		glUniformBlockBinding(m_rendererID, blockIndex, bindingPoint);
	}

private:
	unsigned int CompileShader(unsigned int type, const std::string& source)
	{
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

#ifdef _DEBUG
		// returns in params the value of a parameter for a specific shader object
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)malloc(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			std::cout << "Failed to compile" <<
				(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
			std::cout << message << std::endl;
			glDeleteShader(id);
			return 0;
		}
#endif 
		return id;
	}

	std::tuple<std::string, std::string, std::string> ParseShader(const std::string& vertexShaderPath,
		const std::string& fragmentShaderPath, const std::string& geometryShaderPath)
	{
		std::ifstream vShaderFile(vertexShaderPath);
		std::ifstream fShaderFile(fragmentShaderPath);

		std::stringstream vShaderStream, fShaderStream, gShaderStream;

#ifdef  _DEBUG
		if (!vShaderFile.is_open())  
			std::cout << "failed to read vertex shader file: " << vertexShaderPath; 
		
		if (!fShaderFile.is_open()) 
			std::cout << "failed to open fragment shader file: " << fragmentShaderPath;
#endif 

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		if (!geometryShaderPath.empty()) {
			std::ifstream gShaderFile(geometryShaderPath);

#ifdef _DEBUG
			if (!gShaderFile.is_open())
				std::cout << "failed to open geometry shader file: " << geometryShaderPath;
#endif

			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
		}

		vShaderFile.close();
		fShaderFile.close();

		return std::make_tuple(vShaderStream.str(), fShaderStream.str(), gShaderStream.str());
	}

	unsigned int CreateShader(const std::string& vertexShader,
		const std::string& fragmentShader, const std::string& geometryShader)
	{
		unsigned int program = glCreateProgram();
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
		unsigned int gs;
		if (!geometryShader.empty()) {
			gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
			glAttachShader(program, gs);
		}
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);
		if (!geometryShader.empty())
			glDeleteShader(gs);

		return program;
	}

private:
	unsigned int m_rendererID;
};