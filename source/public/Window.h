#pragma once
#include "RenderObject.h"
#include <vector>

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

private:
	Window();

private:
	struct GLFWwindow* window = nullptr;
	std::vector<RenderObject*> objectsToRender;

public:
	Window(Window const&) = delete;
	void operator=(Window const&) = delete;
};

