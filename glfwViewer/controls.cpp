#include "controls.h"
#include "camera.h"
#include <GL/glfw.h>
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

void Controls::beginFrame(Camera *cp)
{
	const double cur_time = glfwGetTime();
	const double delta_t = cur_time - lastTime_;

	// update camera position
	glm::vec3 position = cp->position();
	if (glfwGetKey(GLFW_KEY_PAGEUP) == GLFW_PRESS)
	{
		position[2] += delta_t * speed;
	}
	if (glfwGetKey(GLFW_KEY_PAGEDOWN) == GLFW_PRESS)
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

