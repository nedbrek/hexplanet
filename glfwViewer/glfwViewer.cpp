#include <GL/glew.h> // must be before gl
#include "../src/hexplanet.h"
#include <GL/glfw.h>
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

	HexPlanet p;
	std::ifstream is("../test/sphere9.fixed.obj");
	p.read(is);

	bool running = true;
	while (running)
	{
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	glfwTerminate();
	return 0;
}

