#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include "Window.h"
#include "Defs.h"
#include <algorithm>
#include "Player.h"
#include "World.h"

void Window::WindowLoop()
{
	float deltaTime, preTime;
	deltaTime = preTime = 0;

	while (glfwWindowShouldClose(window) == false) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		deltaTime = glfwGetTime() - preTime;
		preTime = glfwGetTime();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwMakeContextCurrent(window);

		for(UpdateObject* object : objectsToUpdate)
		{
			if(object->ShouldUpdate())
			{
				object->Update(deltaTime);
			}
		}

		UpdateGUI();

		glClearColor(clearColour.x, clearColour.y, clearColour.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(RenderObject* object : objectsToRender)
		{
			if (object->ShouldDraw())
			{
				object->Draw();
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

Window::Window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(dimensions.x, dimensions.y, "Platonica", nullptr, nullptr);
	clearColour = glm::vec3(0.1f);

	if (!window) {
		puts("Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}


	glfwMakeContextCurrent(window);
	SetVSyncEnabled(enableVsync);
	gladLoadGL();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	ImGui::StyleColorsDark();
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

void Window::SetWireframeEnabled(bool enabled) 
{ 
	glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
}


void Window::UpdateGUI()
{
	ImGui::SetNextWindowPos(ImVec2(dimensions.x, 0), ImGuiCond_Once, ImVec2(1, 0));
	ImGui::SetNextWindowSize(ImVec2(350, 100), ImGuiCond_Once);

	ImGui::Begin("Window");

	bool vsyncEnabled = enableVsync;
	int polygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
	bool wireframeEnabled = polygonMode == GL_LINE;

	if (ImGui::Checkbox("VSync", &vsyncEnabled))
		SetVSyncEnabled(vsyncEnabled);
	if (ImGui::Checkbox("Wireframe", &wireframeEnabled))
		SetWireframeEnabled(wireframeEnabled);
			
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}