#pragma once
#include "RenderObject.h"
#include "UpdateObject.h"
#include <vector>
#include <GLFW/glfw3.h>

class Window
{
public:
	static Window& Instance()
	{
		static Window instance;
		return instance;
	}

	void WindowLoop();

	void StartRenderingObject(RenderObject* object);
	void StopRenderingObject(RenderObject* object);

	void StartUpdatingObject(UpdateObject* object);
	void StopUpdatingObject(UpdateObject* object);

	float GetAspectRatio() const { return (float)dimensions.x / dimensions.y; };
	const glm::ivec2& GetDimensions() const { return dimensions; };

	void SetVSyncEnabled(bool enabled) { enableVsync = enabled;  glfwSwapInterval(enableVsync ? 1 : 0); };
	void SetClearColour(const glm::vec3& inColour) { clearColour = inColour; };
	void SetWireframeEnabled(bool enabled);

private:
	void UpdateGUI();

private:
	Window();
	glm::ivec2 dimensions = glm::ivec2(1920, 1080);
	bool enableVsync = true;
	glm::vec3 clearColour;

private:
	struct GLFWwindow* window = nullptr;
	std::vector<RenderObject*> objectsToRender;
	std::vector<UpdateObject*> objectsToUpdate;

public:
	Window(Window const&) = delete;
	void operator=(Window const&) = delete;
};

