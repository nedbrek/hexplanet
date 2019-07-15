#include <GL/glew.h> // must be before gl
#include "controls.h"
#include "camera.h"
#include "hud.h"
#include "shader.h"
#include "../src/hexplanet.h"
#include "../src/map_data.h"
#include <FTGL/ftgl.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::ostream& operator<<(std::ostream &os, const glm::vec3 &v)
{
	return os << '(' << v[0] << ',' << v[1] << ',' << v[2] << ')';
}

int initGraphics(GLFWwindow *&main_window)
{
	// initialize GLFW
	int rc = glfwInit();
	if (rc != GL_TRUE)
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return 1;
	}

	// open main window
	main_window = glfwCreateWindow(1092, 614, "Main", NULL, NULL);
	if (main_window == NULL)
	{
		std::cerr << "Failed to open GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(main_window);

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
	GLFWwindow *main_window = NULL;
	initGraphics(main_window);

	const GLuint prgIdT = makeShaderProgram("vert.glsl", "frag.glsl");
	if (!prgIdT)
	{
		std::cout << "Failed to load main shaders." << std::endl;
		return 1;
	}

	const GLuint prgIdP = makeShaderProgram("vert.glsl", "plateFrag.glsl");
	if (!prgIdP)
	{
		std::cout << "Failed to load plate shaders." << std::endl;
		return 1;
	}

	// our arrays are:
	// 0 - position
	// 1 - terrain data
	// 2 - plate data

	// default binding is 1, change to 2
	glBindAttribLocation(prgIdP, 2, "terrainData");
	glLinkProgram(prgIdP); // relink after changing binding

	HexPlanet p;
	std::ifstream is("../test/sphere7.fixed.obj");
	p.read(is);

	FILE *terrainFile = fopen("../test/terrain7.bin", "rb");
	MapData<uint8_t> terrainFileData;
	if (!terrainFile || terrainFileData.read(terrainFile))
	{
		printf("Failed to read terrain data\n");
		return 1;
	}

	FILE *plateFile = fopen("../test/plate7.bin", "rb");
	MapData<uint8_t> plateFileData;
	if (!plateFile || plateFileData.read(plateFile))
	{
		printf("Failed to read plate data\n");
		return 1;
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUseProgram(prgIdT);

	GLuint positionVbo;
	glGenBuffers(1, &positionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, p.numTriangles()*3*3*4, NULL, GL_STATIC_DRAW);
	void *positionBufferV = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	float *positionBuffer = static_cast<float*>(positionBufferV);
	assert(positionBuffer);

	std::vector<uint8_t> terrainData;
	std::vector<uint8_t> plateData;
	std::vector<float> positionData;
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

		plateData.push_back(plateFileData[t.m_hexA]);
		plateData.push_back(plateFileData[t.m_hexB]);
		plateData.push_back(plateFileData[t.m_hexC]);

		plateData.push_back(plateFileData[t.m_hexA]);
		plateData.push_back(plateFileData[t.m_hexB]);
		plateData.push_back(plateFileData[t.m_hexC]);

		plateData.push_back(plateFileData[t.m_hexA]);
		plateData.push_back(plateFileData[t.m_hexB]);
		plateData.push_back(plateFileData[t.m_hexC]);

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

	const GLint posAttrib = glGetAttribLocation(prgIdT, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	GLuint terrainTVbo;
	glGenBuffers(1, &terrainTVbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrainTVbo);
	glBufferData(GL_ARRAY_BUFFER, terrainData.size(), &terrainData[0], GL_STATIC_DRAW);

	const GLint tdTAttrib = glGetAttribLocation(prgIdT, "terrainData");
	glVertexAttribIPointer(tdTAttrib, 3, GL_UNSIGNED_BYTE, 0, 0);

	// plate
	glUseProgram(prgIdP);

	GLuint terrainPVbo;
	glGenBuffers(1, &terrainPVbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrainPVbo);
	glBufferData(GL_ARRAY_BUFFER, plateData.size(), &plateData[0], GL_STATIC_DRAW);

	const GLint tdPAttrib = glGetAttribLocation(prgIdP, "terrainData");
	glVertexAttribIPointer(tdPAttrib, 3, GL_UNSIGNED_BYTE, 0, 0);

	Camera camera;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(camera.projection()));

	Controls ctl;

	FTFont *font = new FTGLPixmapFont("/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf");
	font->FaceSize(12);

	Hud hud;
	const size_t varPos = hud.addVarLine("Position: ", "-");

	bool showTerrain = true;
	bool running = true;
	while (running)
	{
		if (showTerrain)
		{
			glDisableVertexAttribArray(tdPAttrib);
			glUseProgram(prgIdT);
			glEnableVertexAttribArray(tdTAttrib);
		}
		else
		{
			glDisableVertexAttribArray(tdTAttrib);
			glUseProgram(prgIdP);
			glEnableVertexAttribArray(tdPAttrib);
		}

		ctl.beginFrame(main_window, &camera);

		hud.updateVarLine(varPos, camera.position());
		hud.render(*font);

		glDrawArrays(GL_TRIANGLES, 0, p.numTriangles()*3);

		glfwSwapBuffers(main_window);
		glfwPollEvents();

		if (glfwGetKey(main_window, GLFW_KEY_P) == GLFW_PRESS)
			showTerrain = !showTerrain;

		running = glfwGetKey(main_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE && !glfwWindowShouldClose(main_window);
	}

	delete font;
	glfwTerminate();
	return 0;
}

