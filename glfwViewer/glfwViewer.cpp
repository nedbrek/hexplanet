#include <GL/glew.h> // must be before gl
#include "shader.h"
#include "../src/hexplanet.h"
#include <GL/glfw.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>

int initGraphics()
{
	// initialize GLFW
	int rc = glfwInit();
	if (rc != GL_TRUE)
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return 1;
	}

	// open main window
	rc = glfwOpenWindow(1092, 614, 8, 8, 8, 8, 32, 0, GLFW_WINDOW);
	if (rc != GL_TRUE)
	{
		std::cerr << "Failed to open GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}
	if (glewInit() != GLEW_OK) // must be after OpenGL context
	{
		std::cerr << "Failed to initialize GLEW." << std::endl;
		return 1;
	}

	glfwSwapInterval(1);

	// cull back faces
	glEnable(GL_CULL_FACE);

	// enable depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	return 0; // success
}

int main(int argc, char **argv)
{
	initGraphics();

	const float fov = 90;
	const float aspectRatio = 16/9.;
	const float nearClipPlane = .1;
	const float farClipPlane = 100;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(glm::perspective(fov, aspectRatio, nearClipPlane, farClipPlane)));
	glm::vec3 position(0, 0, -1);
	glm::vec3 targetPosition(0, 0, 1);
	glm::vec3 headVec(0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(glm::lookAt(position, targetPosition, headVec)));

	const GLuint prgId = makeShaderProgram("vert.glsl", "frag.glsl");
	if (!prgId)
	{
		std::cout << "Failed to load shaders." << std::endl;
		return 1;
	}

	HexPlanet p;
	std::ifstream is("../test/sphere9.fixed.obj");
	p.read(is);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUseProgram(prgId);

	float vertices[] = {
	     0.0f,  0.5f, 0,
	     0.5f, -0.5f, 0,
	    -0.5f, -0.5f, 0
	};
	GLuint vertexVbo;
	glGenBuffers(1, &vertexVbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	const GLint posAttrib = glGetAttribLocation(prgId, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	uint8_t terrainData[9] = {4, 4, 4, 4, 4, 4, 2, 3, 1};
	GLuint terrainVbo;
	glGenBuffers(1, &terrainVbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainData), terrainData, GL_STATIC_DRAW);

	const GLint tdAttrib = glGetAttribLocation(prgId, "terrainData");
	glVertexAttribIPointer(tdAttrib, 3, GL_UNSIGNED_BYTE, 0, 0);
	glEnableVertexAttribArray(tdAttrib);

	GLuint elements[] = {
	    0, 1, 2
	};
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	bool running = true;
	while (running)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	glfwTerminate();
	return 0;
}

