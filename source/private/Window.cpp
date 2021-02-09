#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include "Window.h"
#include "Defs.h"
#include <algorithm>
#include "World.h"
#include "Player.h"

void Window::WindowLoop()
{
	float deltaTime, preTime;
	deltaTime = preTime = 0;

	while (glfwWindowShouldClose(window) == false) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		deltaTime = glfwGetTime() - preTime;
		preTime = glfwGetTime();

		for(UpdateObject* object : objectsToUpdate)
		{
			if(object->ShouldUpdate())
			{
				object->Update(deltaTime);
			}
		}

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(RenderObject* object : objectsToRender)
		{
			if (object->ShouldDraw())
			{
				object->Draw();
			}
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
	dimensions = glm::ivec2(1920, 1080);
	window = glfwCreateWindow(dimensions.x, dimensions.y, "Platonica", nullptr, nullptr);

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
		DPrint("Object already exists in render list.");
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
}

void Window::StartUpdatingObject(UpdateObject* object)
{
	const std::vector<UpdateObject*>::iterator position = std::find(objectsToUpdate.begin(), objectsToUpdate.end(), object);
	if (position != objectsToUpdate.end())
	{
		DPrint("Object already exists in update list.");
	}
	else
	{
		objectsToUpdate.push_back(object);
	}
}

void Window::StopUpdatingObject(UpdateObject* object)
{
	const std::vector<UpdateObject*>::iterator position = std::find(objectsToUpdate.begin(), objectsToUpdate.end(), object);
	if (position != objectsToUpdate.end())
	{
		objectsToUpdate.erase(position);
	}
}