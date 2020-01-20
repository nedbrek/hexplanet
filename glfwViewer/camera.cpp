#include "camera.h"
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

const double PI = std::atan(1)*4;

Camera::Camera()
: pos_(0, 0, -2.5)
, tPos_(0, 0, 0)
, headVec_(0, 1, 0)
, fov_(PI/2)
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

glm::vec3 Camera::targetPosition() const
{
	return tPos_;
}

glm::vec3 Camera::position() const
{
	return pos_;
}

void Camera::setPosition(const glm::vec3 &pos)
{
	pos_ = pos;
}

