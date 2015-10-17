#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
: pos_(0, 0, -2.5)
, tPos_(0, 0, 1)
, headVec_(0, 1, 0)
, fov_(90)
, aspectRatio_(16/9.)
, nearClip_(.1)
, farClip_(100)
{
}

glm::mat4 Camera::projection() const
{
	return glm::perspective(fov_, aspectRatio_, nearClip_, farClip_);
}

glm::mat4 Camera::view() const
{
	return glm::lookAt(pos_, tPos_, headVec_);
}

