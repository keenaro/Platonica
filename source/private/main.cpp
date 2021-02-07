#include "Window.h"
#include "TestObjects.h"
#include "CubeDefs.h"
#include "World.h"

int main()
{
    Window& window = Window::Instance();
	CubeDefs& cubeDefs = CubeDefs::Instance();
	World& world = World::Instance();
	
	world.GenerateChunk(glm::ivec3(0, 0, 0));
	world.GenerateChunk(glm::ivec3(16, 0, 0));

	window.WindowLoop();
}