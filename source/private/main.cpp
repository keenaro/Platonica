#include "Window.h"
#include "TestObjects.h"
#include "CubeDefs.h"
#include "World.h"


int main()
{
    Window& window = Window::Instance();
	CubeDefs& cubeDefs = CubeDefs::Instance();
	World& world = World::Instance();
	
	window.WindowLoop();
}