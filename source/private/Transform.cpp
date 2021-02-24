#include "Transform.h"
#include "glm/gtc/constants.hpp"
#include "glm/geometric.hpp"

glm::vec3 Rotation::GetDirection() const 
{
	return glm::vec3(cos(rotation.y) * sin(rotation.x), sin(rotation.y), cos(rotation.y) * cos(rotation.x));
};

glm::vec3 Rotation::GetRightDirection() const 
{
	const float rightRot = rotation.x - glm::half_pi<float>();
	return glm::cross(GetDirection(), glm::vec3(0, 1, 0));
	//return glm::vec3(sin(rightRot), 0, cos(rightRot));
};

glm::vec3 Rotation::GetUpDirection() const
{
	const float upRot = rotation.y - glm::half_pi<float>();
	return glm::cross(GetRightDirection(), GetDirection());
};