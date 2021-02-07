#include "Transform.h"
#include "glm/gtc/constants.hpp"

glm::vec3 Rotation::GetDirection() const 
{
	return glm::vec3(cos(rotation.y) * sin(rotation.x), sin(rotation.y), cos(rotation.y) * cos(rotation.x));
};

glm::vec3 Rotation::GetRightDirection() const 
{
	const float rightRot = rotation.x - glm::half_pi<float>();
	return glm::vec3(sin(rightRot), 0, cos(rightRot));
};