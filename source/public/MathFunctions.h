#pragma once
#include "glm/vec3.hpp"

static int RoundDownToMultiple(int numToRound, float multiple)
{
	return floor(numToRound / multiple) * multiple;
}

static glm::ivec3 RoundDownToMultiple(const glm::ivec3& vec3ToRound, float multiple)
{
	return glm::ivec3(RoundDownToMultiple(vec3ToRound.x, multiple), RoundDownToMultiple(vec3ToRound.y, multiple), RoundDownToMultiple(vec3ToRound.z, multiple));
}

static glm::ivec3 operator/(const glm::ivec3& lhs, const int& rhs)
{
	return glm::ivec3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

static int WrapMod(int value, int max)
{
	return (value + max * (abs(value / max) + 1)) % max;
}

static glm::ivec3 WrapMod(const glm::ivec3& value, int max)
{
	return glm::ivec3(WrapMod(value.x, max), WrapMod(value.y, max), WrapMod(value.z, max));
}