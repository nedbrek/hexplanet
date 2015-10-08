#include <GL/glew.h> // must be before gl
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

	const float fov = 90;
	const float aspectRatio = 16/9.;
	const float nearClipPlane = .1;
	const float farClipPlane = 100;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(glm::perspective(fov, aspectRatio, nearClipPlane, farClipPlane)));
	glm::vec3 position(0, 0, -2.5);
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
	std::ifstream is("../test/sphere6.fixed.obj");
	p.read(is);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUseProgram(prgId);

	const size_t numVerts = p.getNumHexes();

	GLuint textureId;
	glGenTextures(1, &textureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// turn off mipmapping
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, numVerts, 0, GL_RGB, GL_FLOAT, &p.hex(0).m_vertPos);
	glUniform1i(glGetUniformLocation(prgId, "positionData"), 0);

	FILE *terrainFile = fopen("../test/terrain6.bin", "rb");
	MapData<uint8_t> terrainFileData;
	terrainFileData.read(terrainFile);

	std::vector<uint32_t> indexes;
	std::vector<uint8_t> terrainData;
	for (size_t i = 0; i < p.numTriangles(); ++i)
	{
		const HexTri &t = p.triangle(i);
		assert(0 <= t.m_hexA && t.m_hexA < p.getNumHexes());
		indexes.push_back(t.m_hexA);
		indexes.push_back(t.m_hexB);
		indexes.push_back(t.m_hexC);

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
	GLuint terrainVbo;
	glGenBuffers(1, &terrainVbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVbo);
	glBufferData(GL_ARRAY_BUFFER, terrainData.size(), &terrainData[0], GL_STATIC_DRAW);

	const GLint tdAttrib = glGetAttribLocation(prgId, "terrainData");
	glVertexAttribIPointer(tdAttrib, 3, GL_UNSIGNED_BYTE, 0, 0);
	glEnableVertexAttribArray(tdAttrib);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ARRAY_BUFFER, ebo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t)*indexes.size(), &indexes[0], GL_STATIC_DRAW);

	const GLint idxAttrib = glGetAttribLocation(prgId, "index");
	glVertexAttribIPointer(idxAttrib, 1, GL_INT, 0, 0);
	glEnableVertexAttribArray(idxAttrib);

	bool running = true;
	while (running)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, indexes.size());
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	glfwTerminate();
	return 0;
}

