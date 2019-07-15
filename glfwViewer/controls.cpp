#include "controls.h"
#include "camera.h"
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>

namespace
{
	const float speed = .5; // units per second
}

//----------------------------------------------------------------------------
Controls::Controls()
: lastTime_(glfwGetTime())
{
}

void Controls::beginFrame(GLFWwindow *main_window, Camera *cp)
{
	const double cur_time = glfwGetTime();
	const double delta_t = cur_time - lastTime_;

	// update camera position
	glm::vec3 position = cp->position();
	if (glfwGetKey(main_window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		position[2] += delta_t * speed;
	}
	if (glfwGetKey(main_window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		position[2] -= delta_t * speed;
	}
	cp->setPosition(position);

	lastTime_ = cur_time;

	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(cp->view()));
}

