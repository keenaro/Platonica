#include "GameManager.h"
#include "WorldLoader.h"
#include "CubeDefs.h"
#include "World.h"

GameManager::GameManager()
{
	cubeDefs = MakeShared<CubeDefs>();
	worldLoader = MakeShared<WorldLoader>();
}

void GameManager::Exit()
{
	if(world)
	{
		world->Exit();
	}
}