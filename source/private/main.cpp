#include "Window.h"
#include "TestObjects.h"
#include "CubeDefs.h"
#include "World.h"


int main()
{
    Window& window = Window::Instance();
	CubeDefs& cubeDefs = CubeDefs::Instance();
	World& world = World::Instance();

	const int length = 16;
	for(int z = -length; z < length; z++)
	{
		for (int x = -length; x < length; x++)
		{
			world.chunkManager.RequestTask(glm::ivec3(0, 0, 0), glm::ivec3(x*16, -16, z*16));
		}
	}

	window.WindowLoop();
}