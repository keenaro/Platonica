#include "Player.h"
#include "GLFW/glfw3.h"

Player::Player(Camera& camera) : Camera(camera), UpdateObject(false)
{
	movementSpeed = glm::vec3(2.f, 2.f, 2.f);
	rotationSpeed = glm::vec3(1.5f, 1.5f, 1.5f);
	position = glm::vec3(0.0f, 0.0f, -10.f);
}

void Player::Rotate(glm::vec3& rotate)
{
	Camera::Rotate(rotate);
	const int sign = (0 < rotation.y) - (rotation.y < 0);
	rotation.y = abs(rotation.y) > 1.5 ? 1.5 * sign : rotation.y;
}

void Player::Update(float deltaTime)
{
	ProcessJoystick(deltaTime);
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
			speedIncrease = 10.0f;
		}
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