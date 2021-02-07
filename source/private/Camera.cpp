#include "Camera.h"
#include "Window.h"

glm::mat4 Camera::GetProjectionXForm() const
{
	return glm::perspective(
		glm::radians(fov),
		Window::Instance().GetAspectRatio(),
		nearPlaneDistance,
		farPlaceDistance);
}