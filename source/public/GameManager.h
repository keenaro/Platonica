#pragma once
#include "Defs.h"
#include "UpdateObject.h"

class World;
class WorldLoader;
class CubeDefs;

class GameManager
{

public:
	static GameManager& Instance()
	{
		static GameManager instance;
		return instance;
	}

	GameManager();

public:
	SharedPtr<World> GetWorld() const { return world; }
	SharedPtr<WorldLoader> GetWorldLoader() const { return worldLoader; }
	SharedPtr<CubeDefs> GetCubeDefs() const { return cubeDefs; }

	void SetWorld(SharedPtr<World> newWorld) { world = newWorld; };
	void ClearWorld() { world = nullptr; };
	void ClearWorldLoader() { worldLoader = nullptr; };

	void Exit();

private:
	SharedPtr<World> world = nullptr;
	SharedPtr<WorldLoader> worldLoader = nullptr;
	SharedPtr<CubeDefs> cubeDefs = nullptr;

public:
	GameManager(GameManager const&) = delete;
	void operator=(GameManager const&) = delete;
};