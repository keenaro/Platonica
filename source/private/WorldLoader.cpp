#include "WorldLoader.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <direct.h>
#include "World.h"
#include "GameManager.h"
#include "Window.h"
#include "ClientWorld.h"

WorldLoader::WorldLoader() : UpdateObject(true)
{
	_mkdir("Worlds");

	String path = "./Worlds/";

	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		if (file.is_directory())
		{
			worldNames.push_back(file.path().filename().string());
			DPrintf("Found world: %s\n", worldNames.back().c_str());
		}
	}
}

void WorldLoader::Update(float deltaTime)
{
	UpdateGUI();
}

void WorldLoader::UpdateGUI()
{
	const auto windowDimensions = Window::Instance().GetDimensions();
	ImGui::SetNextWindowPos(ImVec2((float)windowDimensions.x / 2, (float)windowDimensions.y / 2), ImGuiCond_Once, ImVec2(0.5, 0.5));
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Once);

	ImGui::Begin("World Loader");

	if (ImGui::CollapsingHeader("Host World"))
	{
		for (auto it = worldNames.begin(); it != worldNames.end(); it++)
		{
			if (ImGui::Button(it->c_str())) CreateOrLoadWorld(*it);
			ImGui::SameLine(0, 10);
			String deleteString = "Delete##" + *it;
			if (ImGui::Button(deleteString.c_str())) {
				DeleteWorld(it);
				break;
			}
		}

		if (ImGui::CollapsingHeader("Create New World"))
		{
			static char worldName[20] = "NewWorld";
			static int seed = rand();
			static int regionLength = 8;
			ImGui::InputText("World Name", worldName, 20);
			ImGui::InputInt("Seed", &seed);
			ImGui::InputInt("World Length", &regionLength);
			regionLength += regionLength % 2 != 0.0f;
			if (ImGui::Button("Create World!")) CreateOrLoadWorld(worldName, seed, regionLength);
		}
	}
	if (ImGui::CollapsingHeader("Join World"))
	{
		static char hostname[15] = "localhost";
		static int port = 25565;
		ImGui::InputText("Hostname", hostname, 15);
		ImGui::InputInt("Port", &port);
		if (ImGui::Button("Connect to World!")) ConnectToWorld(hostname, port);
	}

	ImGui::End();
}

void WorldLoader::CreateOrLoadWorld(const String& worldName, int seed, int regionLength)
{
	auto& gameManager = GameManager::Instance();
	gameManager.SetWorld(MakeShared<World>(worldName, seed, regionLength));
	Window::Instance().StopUpdatingObject(this);
}

void WorldLoader::ConnectToWorld(const char* hostname, int port)
{
	auto& gameManager = GameManager::Instance();
	ClientWorld testConnectWorld;
	if (testConnectWorld.TryConnect(hostname, port))
	{
		gameManager.SetWorld(MakeShared<ClientWorld>(testConnectWorld));
		Window::Instance().StopUpdatingObject(this);
	}
	else
	{
		DPrintf("Failed to connect to %s:%i.\n", hostname, port);
	}

}

void WorldLoader::DeleteWorld(std::list<String>::const_iterator& world)
{
	String worldPath = "./Worlds/" + *world;
	std::filesystem::remove_all(worldPath);
	world = worldNames.erase(world);
}