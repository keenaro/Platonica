#pragma once
#include "Transform.h"
#include "glm/mat4x4.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Camera : public Transform<glm::vec3>
{
public:
	Camera(Transform<glm::vec3>& inTransform = Transform(), float inFov = 90.f, float inNearPlaneDistance = 0.1f, float inFarPlaneDistance = 10000.f, float inAspectRatio = 1920.f / 1080.f) : 
		Transform(inTransform), fov(inFov), nearPlaneDistance(inNearPlaneDistance), farPlaceDistance(inFarPlaneDistance) { }

	glm::mat4 GetViewXForm() const { return glm::lookAt(position, position + GetDirection(), glm::vec3(0, 1, 0)); };
	glm::mat4 GetProjectionXForm() const;

protected:
	float fov{ 90.f };
	float nearPlaneDistance{ 0.1f };
	float farPlaceDistance{ 10000.f };
};