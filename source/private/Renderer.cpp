#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include "Renderer.h"
#include "Defs.h"
#include <algorithm>


void Renderer::WindowLoop()
{
	while (glfwWindowShouldClose(window) == false) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void Renderer::Initialise()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(1920, 1080, "Platonica", nullptr, nullptr);

	if (!window) {
		puts("Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
}

void Renderer::StartRenderingObject(const RenderObject* object)
{
	const std::vector<const RenderObject*>::iterator position = std::find(objectsToRender.begin(), objectsToRender.end(), object);
	if(position != objectsToRender.end())
	{
		DPRINT("Object already exists in render list.");
	}
	else
	{
		objectsToRender.push_back(object);
	}
}

void Renderer::StopRenderingObject(const RenderObject* object)
{
	const std::vector<const RenderObject*>::iterator position = std::find(objectsToRender.begin(), objectsToRender.end(), object);
	if (position != objectsToRender.end())
	{
		objectsToRender.erase(position);
	}
	else 
	{
		DPRINT("Object does not exist in render list.");
	}
}