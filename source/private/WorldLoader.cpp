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
	ImGui::Begin("World Loader");

	if (ImGui::CollapsingHeader("Host World"))
	{
		for(const auto& worldName : worldNames)
		{
			if (ImGui::Button(worldName.c_str())) CreateOrLoadWorld(worldName);
		}

		if (ImGui::CollapsingHeader("Create New World"))
		{
			static char worldName[20] = "NewWorld";
			static int seed = rand();
			static int regionLength = 8;
			ImGui::InputText("World Name", worldName, 20);
			ImGui::InputInt("Seed", &seed);
			ImGui::InputInt("World Length", &regionLength);
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