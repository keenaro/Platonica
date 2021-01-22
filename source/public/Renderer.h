#pragma once
#include "RenderObject.h"
#include <vector>

class Renderer
{
public:
	static Renderer& Instance()
	{
		static Renderer instance;
		return instance;
	}

	void Initialise();
	void WindowLoop();

	void StartRenderingObject(const RenderObject* object);
	void StopRenderingObject(const RenderObject* object);

private:
	Renderer() {};

private:
	struct GLFWwindow* window = nullptr;
	std::vector<const RenderObject*> objectsToRender;

public:
	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;
};

