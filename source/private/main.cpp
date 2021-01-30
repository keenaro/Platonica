#include "Window.h"
#include "TestObjects.h"

int main()
{
    Window& window = Window::Instance();
	TriangleObject test;

	window.WindowLoop();
}