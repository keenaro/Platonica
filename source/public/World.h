#pragma once
#include "RenderObject.h"
#include <vector>

class World
{
public:
	static World& Instance()
	{
		static World instance;
		return instance;
	}

private:
	World();

public:
	World(World const&) = delete;
	void operator=(World const&) = delete;
};

