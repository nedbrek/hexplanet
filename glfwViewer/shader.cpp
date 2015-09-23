#include "shader.h"
#include <fstream>
#include <iostream>

std::string fileToString(const std::string &filename)
{
	std::ifstream fs(filename.c_str());
	if (!fs.is_open())
	{
		std::cerr << "Unable to open shader " << filename << '.'
		  << std::endl;
		return 0; // invalid id
	}

	std::string fileString;
	std::string line;
	while (std::getline(fs, line))
	{
		fileString += line;
		fileString += '\n';
	}

	return fileString;
}

GLuint compileShader(GLenum type, const std::string &filename)
{
	// allocate program resource
	GLuint shaderId = glCreateShader(type);

	// pull in source code
	const std::string fileString = fileToString(filename);

	// compile it
	const char *source = fileString.c_str();
	glShaderSource(shaderId, 1, &source, NULL);
	glCompileShader(shaderId);

	// check results
	GLint result = GL_FALSE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE)
	{
		int infoLogLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::string infoLog(infoLogLength+1, 0);
		glGetShaderInfoLog(shaderId, infoLogLength, NULL, &infoLog[0]);
		std::cerr << "Shader " << std::string(filename)
		  << " compilation failed." << std::endl
		  << infoLog.c_str() << std::endl;
		return 0; // invalid id
	}
	return shaderId;
}

GLuint makeShaderProgram(const std::string &vertFilename, const std::string &fragFilename)
{
	// allocate program resource
	// vertex shader
	const GLuint vertexShaderId = compileShader(GL_VERTEX_SHADER, vertFilename);
	if (vertexShaderId == 0)
		return 0;

	// fragment shader
	const GLuint fragmentShaderId = compileShader(GL_FRAGMENT_SHADER, fragFilename);
	if (fragmentShaderId == 0)
	{
		glDeleteShader(vertexShaderId);
		return 0;
	}

	// link the program
	const GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	// done with the shader objects
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	// check results
	GLint result = GL_FALSE;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	if (result != GL_TRUE)
	{
		int infoLogLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::cerr << "Shader Program link failed." << std::endl;
		return 0; // invalid id
	}

	return programId;
}

