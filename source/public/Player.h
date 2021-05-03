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
	SharedPtr<SquareObject> reticle;
	CubeID currentlySelectedBlock = Stone;
	glm::vec3 lastReplicatedPosition = glm::vec3(0);
};

class NetworkPlayer : public Transform<glm::vec3>
{
public:
	NetworkPlayer(ENetPeer* netPeer, enet_uint32 inUniqueId);

public:
	void DrawPlayer();
	ENetPeer* GetNetPeer() const { return netPeer; };
	void SetNetPeer(ENetPeer* inNetPeer) { netPeer = inNetPeer; };
	const enet_uint32& GetUniqueID() const { return uniqueId; };

private:
	SharedPtr<BoxObject> head;
	SharedPtr<BoxObject> body;
	SharedPtr<BoxObject> leftLeg;
	SharedPtr<BoxObject> rightLeg;
	SharedPtr<BoxObject> leftArm;
	SharedPtr<BoxObject> rightArm;

	ENetPeer* netPeer;
	enet_uint32 uniqueId = 0;
};
