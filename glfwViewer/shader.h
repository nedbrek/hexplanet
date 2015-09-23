#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

GLuint makeShaderProgram(const std::string &vertFilename, const std::string &fragFilename);

#endif

