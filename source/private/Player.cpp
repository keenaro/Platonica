#include "Player.h"
#include "GLFW/glfw3.h"
#include "World.h"
#include "Window.h"
#include "ChunkRegion.h"
#include "GameManager.h"

Player::Player(Camera& camera) : Camera(camera), UpdateObject(false)
{
	movementSpeed = glm::vec3(2.f, 2.f, 2.f);
	rotationSpeed = glm::vec3(1.5f, 1.5f, 1.5f);
	position = glm::vec3(0, 32, 0);
	rotation = glm::vec3(glm::two_pi<float>() * 0.9f, -0.1f, 0.f);
	reticle = MakeShared<SquareObject>(glm::vec2(1, Window::Instance().GetAspectRatio()) * 0.002f, true);
}

void Player::Rotate(glm::vec3& rotate)
{
	Camera::Rotate(rotate);
	const int sign = glm::sign(rotation.y);
	const float halfPi = glm::half_pi<float>() - 0.001f;
	rotation.y = abs(rotation.y) > halfPi ? halfPi * sign : rotation.y;
}

void Player::Update(float deltaTime)
{
	ProcessJoystick(deltaTime);
	if(glm::distance(lastReplicatedPosition, position) > 0.2f)
	{
		lastReplicatedPosition = position;
		GameManager::Instance().GetWorld()->SendLocalPlayerPosition();
	}

	UpdateGUI();
}

void Player::ProcessJoystick(float deltaTime)
{
	float speedIncrease = 1.f;

	int count;

	const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
	if (count)
	{
		auto world = GameManager::Instance().GetWorld();

		if (buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS)
		{
			Move(glm::vec3(0, 1, 0) * GetMovementSpeed().y * deltaTime);
		}
		if (buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS)
		{
			Move(glm::vec3(0, -1, 0) * GetMovementSpeed().y * deltaTime);
		}
		if (buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS)
		{
			speedIncrease = speedIncreaseMulitplier;
		}

		//#Needs reworking.
		static bool holdingA = false;
		if (buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS)
		{
			if (!holdingA)
			{
				world->PlaceBlockFromPositionInDirection(GetPosition(), GetDirection(), GetReach(), currentlySelectedBlock);
			}
			holdingA = true;
		}
		else holdingA = false;

		static bool holdingX = false;
		if (buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS)
		{
			if (!holdingX)
			{
				world->PlaceBlockFromPositionInDirection(GetPosition(), GetDirection(), GetReach(), CubeID::Air, true);
			}
			holdingX = true;
		}
		else holdingX = false;
	}

	const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
	if (count)
	{
		const float deadzone = 0.5f;

		if (abs(axes[GLFW_GAMEPAD_AXIS_RIGHT_X]) > deadzone || abs(axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]) > deadzone)
		{
			Rotate(glm::vec3(GetRotationSpeed().x * axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * deltaTime, GetRotationSpeed().y * axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * deltaTime, 0.f));
		}

		if (abs(axes[GLFW_GAMEPAD_AXIS_LEFT_X]) > deadzone)
		{
			Move(GetRightDirection() * axes[GLFW_GAMEPAD_AXIS_LEFT_X] * GetMovementSpeed().x * deltaTime * speedIncrease);
		}

		if (abs(axes[GLFW_GAMEPAD_AXIS_LEFT_Y]) > deadzone)
		{
			Move(GetDirection() * axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * GetMovementSpeed().z * deltaTime * -1.0f * speedIncrease);
		}
	}	
}

void Player::UpdateGUI()
{
	ImGui::Begin("Player");

	ImGui::PushItemWidth(100);
	ImGui::SliderFloat("Speed Increase Multiplier", &speedIncreaseMulitplier, 10.0f, 100.0f);

	const auto cubeDefs = GameManager::Instance().GetCubeDefs();
	
	if (ImGui::BeginCombo("##SelectedItem", cubeDefs->GetCubeDef(currentlySelectedBlock).GetName().c_str()))
	{
		auto cubeDefsMap = cubeDefs->GetCubeDefsMap();

		for (auto const& [key, val] : cubeDefsMap)
		{
			bool isSelected = (key == currentlySelectedBlock);
			if (ImGui::Selectable(val.GetName().c_str(), isSelected))
				currentlySelectedBlock = key;
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine(0, 10.f);
	ImGui::Text("Selected Block");

	ImGui::Text("Position: %s", glm::to_string(position).c_str());
	ImGui::Text("Rotation: %s", glm::to_string(GetDirection()).c_str());


	ImGui::End();
}

NetworkPlayer::NetworkPlayer(ENetPeer* inNetPeer, enet_uint32 inUniqueId) : Transform(Transform()), netPeer(inNetPeer), uniqueId(inUniqueId)
{
	const glm::vec3 headSize(0.4f);
	const glm::vec3 bodySize(0.2f, 0.6f, 0.4f);
	const glm::vec3 armSize(0.2f, 0.6f, 0.2f);
	const glm::vec3 legSize(0.2f, 0.6f, 0.2f);

	head = MakeShared<BoxObject>(glm::vec3(0), headSize, glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), String("RegionEntity"), false);
	const auto& bodyHeightOffset = -(headSize.y + bodySize.y) / 2;
	body = MakeShared<BoxObject>(glm::vec3(0, bodyHeightOffset, 0), bodySize, glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), String("RegionEntity"), false);
	leftLeg = MakeShared<BoxObject>(glm::vec3(0, bodyHeightOffset - (bodySize.y + legSize.y)/2, legSize.z/2), legSize, glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), String("RegionEntity"), false);
	rightLeg = MakeShared<BoxObject>(glm::vec3(0, bodyHeightOffset - (bodySize.y + legSize.y)/2, -legSize.z/2), legSize, glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), String("RegionEntity"), false);
	leftArm = MakeShared<BoxObject>(glm::vec3(0, -(headSize.y+armSize.y)/2, (bodySize.z+armSize.z)/2), armSize, glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), String("RegionEntity"), false);
	rightArm = MakeShared<BoxObject>(glm::vec3(0, -(headSize.y+armSize.y)/2, -(bodySize.z+armSize.z)/2), armSize, glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), String("RegionEntity"), false);

	DPrint("Registered Network Player.");
}

void NetworkPlayer::DrawPlayer()
{
	const auto world = GameManager::Instance().GetWorld();
	const auto regions = world->GetRegions();

	const auto wrappedPlayerPosition = world->TranslateIntoWrappedWorld(position);

	auto shader = head->GetShader();
	shader->SetVector3("Position", wrappedPlayerPosition);
	shader->SetVector3("Rotation", rotation);

	for(auto const& [key, val] : regions)
	{
		shader->SetIVector3("RegionPosition", val->GetWorldPosition());
		head->Draw();
		body->Draw();
		leftLeg->Draw();
		rightLeg->Draw();
		leftArm->Draw();
		rightArm->Draw();
	}
}
