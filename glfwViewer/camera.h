#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

/// wrapper for matrix operations related to the view
class Camera
{
public:
	Camera();

	///@return matrix for 2d projection to window space
	glm::mat4 projection() const;

	///@return matrix for projection to view space
	glm::mat4 view() const;

private:
	glm::vec3 pos_; ///< position in world space
	glm::vec3 tPos_; ///< position of what we're looking at (world space)
	glm::vec3 headVec_; ///< head vector (up)
	float fov_; ///< field of view
	float aspectRatio_; ///< window width / height
	float nearClip_; ///< near clip plane
	float farClip_; ///< far clip plane
};

#endif

