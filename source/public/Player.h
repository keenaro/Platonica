#pragma once
#include "Camera.h"
#include "UpdateObject.h"
#include "TestObjects.h"
#include "CubeDefs.h"

class Player : public Camera, public UpdateObject
{
public:
	Player(Camera& camera = Camera());

public:
	void Rotate(glm::vec3& rotate) override;
	void Update(float deltaTime) override;
	const glm::vec3& GetMovementSpeed() const { return movementSpeed; };
	const glm::vec3& GetRotationSpeed() const { return rotationSpeed; };
	int GetReach() const { return reach; };
	void UpdateGUI();

private:
	void ProcessJoystick(float deltaTime);

private:
	glm::vec3 movementSpeed;
	float speedIncreaseMulitplier = 10.0f;
	glm::vec3 rotationSpeed;
	int reach = 7;
	SharedPtr<BoxObject> reticle;
	CubeID currentlySelectedBlock = Stone;
};
