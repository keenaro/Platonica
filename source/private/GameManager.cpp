#include "GameManager.h"
#include "WorldLoader.h"
#include "CubeDefs.h"
#include "World.h"

GameManager::GameManager()
{
	cubeDefs = MakeShared<CubeDefs>();
	CreateWorldLoader();
}

void GameManager::Exit()
{
	if(world)
	{
		world->Exit();
	}
}

void GameManager::CreateWorldLoader() 
{ 
	worldLoader = MakeShared<WorldLoader>(); 
}