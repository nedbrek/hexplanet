#include "controls.h"
#include "camera.h"
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>

namespace
{
	const float speed = .5; // units per second
	const float PI = atan(1.)*4;
	const float TWO_PI = PI*2;
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
		dist_ += delta_t * speed;
		constexpr float min_z = -1.0 - cp->nearClip();
		if (dist_ > min_z)
			dist_ = min_z;
	}
	if (glfwGetKey(main_window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		dist_ -= delta_t * speed;
	}
	if (glfwGetKey(main_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		theta_ += delta_t * speed;
		if (theta_ > TWO_PI)
			theta_ -= TWO_PI;
	}
	if (glfwGetKey(main_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		theta_ -= delta_t * speed;
		if (theta_ < 0)
			theta_ += TWO_PI;
	}
	position[0] = dist_ * sin(theta_);
	position[2] = dist_ * cos(theta_);

	cp->setPosition(position);

	lastTime_ = cur_time;

	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(cp->view()));
}

