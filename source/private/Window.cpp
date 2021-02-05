#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include "Window.h"
#include "Defs.h"
#include <algorithm>


void Window::WindowLoop()
{
	while (glfwWindowShouldClose(window) == false) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(RenderObject* object : objectsToRender)
		{
			object->Draw();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

Window::Window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(1080, 1080, "Platonica", nullptr, nullptr);

	if (!window) {
		puts("Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void Window::StartRenderingObject(RenderObject* object)
{
	const std::vector<RenderObject*>::iterator position = std::find(objectsToRender.begin(), objectsToRender.end(), object);
	if(position != objectsToRender.end())
	{
		DPRINT("Object already exists in render list.");
	}
	else
	{
		objectsToRender.push_back(object);
	}
}

void Window::StopRenderingObject(RenderObject* object)
{
	const std::vector<RenderObject*>::iterator position = std::find(objectsToRender.begin(), objectsToRender.end(), object);
	if (position != objectsToRender.end())
	{
		objectsToRender.erase(position);
	}
	else 
	{
		DPRINT("Object does not exist in render list.");
	}
}