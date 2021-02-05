#include "Window.h"
#include "TestObjects.h"
#include "CubeDefs.h"
#include "Chunk.h"

int main()
{
    Window& window = Window::Instance();
	TriangleObject test;
	CubeDefs& cubeDefs = CubeDefs::Instance();
	Chunk chunk();

	window.WindowLoop();
}