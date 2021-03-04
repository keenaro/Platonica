#pragma once
#include "glm/vec3.hpp"

int RoundDownToMultiple(int numToRound, float multiple)
{
	return floor(numToRound / multiple) * multiple;
}

glm::ivec3 RoundDownToMultiple(const glm::ivec3& vec3ToRound, float multiple)
{
	return glm::ivec3(RoundDownToMultiple(vec3ToRound.x, multiple), RoundDownToMultiple(vec3ToRound.y, multiple), RoundDownToMultiple(vec3ToRound.z, multiple));
}

glm::ivec3 operator/(const glm::ivec3& lhs, const int& rhs)
{
	return glm::ivec3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}