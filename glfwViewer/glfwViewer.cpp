#include <GL/glew.h> // must be before gl
#include "camera.h"
#include "shader.h"
#include "../src/hexplanet.h"
#include "../src/map_data.h"
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

	Camera camera;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(camera.projection()));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(camera.view()));

	const GLuint prgId = makeShaderProgram("vert.glsl", "frag.glsl");
	if (!prgId)
	{
		std::cout << "Failed to load shaders." << std::endl;
		return 1;
	}

	HexPlanet p;
	std::ifstream is("../test/sphere7.fixed.obj");
	p.read(is);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUseProgram(prgId);

	GLuint positionVbo;
	glGenBuffers(1, &positionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, p.numTriangles()*3*3*4, NULL, GL_STATIC_DRAW);
	void *positionBufferV = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	float *positionBuffer = static_cast<float*>(positionBufferV);
	assert(positionBuffer);

	FILE *terrainFile = fopen("../test/terrain7.bin", "rb");
	MapData<uint8_t> terrainFileData;
	if (!terrainFile || terrainFileData.read(terrainFile))
	{
		printf("Failed to read terrain data\n");
		return 1;
	}

	std::vector<uint8_t> terrainData;
	for (size_t i = 0; i < p.numTriangles(); ++i)
	{
		const HexTri &t = p.triangle(i);
		assert(0 <= t.m_hexA && t.m_hexA < p.getNumHexes());

		// copy in position data (we're doing the indirection here)
		*positionBuffer = p.hex(t.m_hexA).m_vertPos.x; ++positionBuffer;
		*positionBuffer = p.hex(t.m_hexA).m_vertPos.y; ++positionBuffer;
		*positionBuffer = p.hex(t.m_hexA).m_vertPos.z; ++positionBuffer;

		*positionBuffer = p.hex(t.m_hexB).m_vertPos.x; ++positionBuffer;
		*positionBuffer = p.hex(t.m_hexB).m_vertPos.y; ++positionBuffer;
		*positionBuffer = p.hex(t.m_hexB).m_vertPos.z; ++positionBuffer;

		*positionBuffer = p.hex(t.m_hexC).m_vertPos.x; ++positionBuffer;
		*positionBuffer = p.hex(t.m_hexC).m_vertPos.y; ++positionBuffer;
		*positionBuffer = p.hex(t.m_hexC).m_vertPos.z; ++positionBuffer;

		terrainData.push_back(terrainFileData[t.m_hexA]);
		terrainData.push_back(terrainFileData[t.m_hexB]);
		terrainData.push_back(terrainFileData[t.m_hexC]);

		terrainData.push_back(terrainFileData[t.m_hexA]);
		terrainData.push_back(terrainFileData[t.m_hexB]);
		terrainData.push_back(terrainFileData[t.m_hexC]);

		terrainData.push_back(terrainFileData[t.m_hexA]);
		terrainData.push_back(terrainFileData[t.m_hexB]);
		terrainData.push_back(terrainFileData[t.m_hexC]);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	const GLint posAttrib = glGetAttribLocation(prgId, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	GLuint terrainVbo;
	glGenBuffers(1, &terrainVbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVbo);
	glBufferData(GL_ARRAY_BUFFER, terrainData.size(), &terrainData[0], GL_STATIC_DRAW);

	const GLint tdAttrib = glGetAttribLocation(prgId, "terrainData");
	glVertexAttribIPointer(tdAttrib, 3, GL_UNSIGNED_BYTE, 0, 0);
	glEnableVertexAttribArray(tdAttrib);

	bool running = true;
	while (running)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, p.numTriangles()*3);
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	glfwTerminate();
	return 0;
}

