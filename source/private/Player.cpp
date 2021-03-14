#include "Player.h"
#include "GLFW/glfw3.h"
#include "World.h"
#include "Window.h"

Player::Player(Camera& camera) : Camera(camera), UpdateObject(false)
{
	movementSpeed = glm::vec3(2.f, 2.f, 2.f);
	rotationSpeed = glm::vec3(1.5f, 1.5f, 1.5f);
	position = glm::vec3(0, 32, 0);
	rotation = glm::vec3(glm::two_pi<float>() * 0.9f, -0.1f, 0.f);
	reticle = MakeShared<BoxObject>(glm::vec2(1, Window::Instance().GetAspectRatio()) * 0.002f, true);
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
	UpdateGUI();
}

void Player::ProcessJoystick(float deltaTime)
{
	float speedIncrease = 1.f;

	int count;

	const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
	if (count)
	{
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
				World::Instance().PlaceBlockFromPositionInDirection(GetPosition(), GetDirection(), GetReach(), CubeID::Stone);
			}
			holdingA = true;
		}
		else holdingA = false;

		static bool holdingX = false;
		if (buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS)
		{
			if (!holdingX)
			{
				World::Instance().PlaceBlockFromPositionInDirection(GetPosition(), GetDirection(), GetReach(), CubeID::Air, true);
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

	ImGui::Text("Position: %s", glm::to_string(position).c_str());
	ImGui::Text("Rotation: %s", glm::to_string(GetDirection()).c_str());

	ImGui::End();
}