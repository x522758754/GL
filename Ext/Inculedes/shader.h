#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	unsigned int ID;
	Shader()
	{

	}

	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
	{
		std::string vertexCode, fragmentCode, geometryCode;
		std::ifstream vsFile, fsFile, gsFile;

		vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vsFile.open(vertexPath);
			fsFile.open(fragmentPath);
			std::stringstream vsStream, fsStream, gsStream;
			vsStream << vsFile.rdbuf();
			fsStream << fsFile.rdbuf();
			vsFile.close();
			fsFile.close();

			vertexCode = vsStream.str();
			fragmentCode = fsStream.str();

			if (nullptr != geometryPath)
			{
				gsFile.open(geometryPath);
				gsStream << gsFile.rdbuf();
				gsFile.close();
				geometryCode = gsStream.str();

			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR: shader file: " << vertexPath << " not successfully read.  " << std::endl;
		}

		const char* vsCode = vertexCode.c_str();
		const char* fsCode = fragmentCode.c_str();

		unsigned int vertex, fragment, geometry;
		createShader(&vertex, GL_VERTEX_SHADER, &vsCode);
		createShader(&fragment, GL_FRAGMENT_SHADER, &fsCode);

		this->ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (nullptr != geometryPath)
		{
			const char* gsCode = geometryCode.c_str();
			createShader(&geometry, GL_GEOMETRY_SHADER, &gsCode);
			glAttachShader(ID, geometry);
		}
		glLinkProgram(this->ID);

		int success, length;
		glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			char *infoLog = nullptr;
			glGetProgramiv(this->ID, GL_INFO_LOG_LENGTH, &length);
			infoLog = new char[length];
			glGetProgramInfoLog(this->ID, length, NULL, infoLog);
			std::cout << "Error; " << infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use()
	{
		glUseProgram(this->ID);
	}

	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec2(const std::string &name, glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec3(const std::string &name, const glm::vec3 & vec3) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vec3[0]);
	}

	void setMat4(const std::string &name, const glm::mat4 &mat4) const
	{
		//&mat4[0][0]
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat4));
	}

	void setArrayVec3(const std::string &name, const glm::vec3 arrayVec3[]) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), arrayVec3->length(), &arrayVec3[0][0]);
	}

private:

	void createShader(unsigned int* shaderObj, unsigned int shaderType, const char* const* string) const
	{
		*shaderObj = glCreateShader(shaderType);
		glShaderSource(*shaderObj, 1, string, NULL);
		glCompileShader(*shaderObj);

		int success, length;
		glGetShaderiv(*shaderObj, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char *infoLog = nullptr;
			glGetShaderiv(*shaderObj, GL_INFO_LOG_LENGTH, &length);
			infoLog = new char[length];
			glGetShaderInfoLog(*shaderObj, length, NULL, infoLog);
			std::cout << "Error: " << infoLog << std::endl;
		}
	}
};

#endif // !SHADER_H

