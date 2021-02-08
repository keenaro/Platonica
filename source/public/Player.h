#pragma once
#include "Camera.h"
#include "UpdateObject.h"

class Player : public Camera, public UpdateObject
{
public:
	Player(Camera& camera = Camera());

public:
	void Rotate(glm::vec3& rotate) override;
	void Update(float deltaTime) override;
	const glm::vec3& GetMovementSpeed() const { return movementSpeed; };
	const glm::vec3& GetRotationSpeed() const { return rotationSpeed; };

private:
	void ProcessJoystick(float deltaTime);

private:
	glm::vec3 movementSpeed;
	glm::vec3 rotationSpeed;
};