#define ENET_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "Defs.h"
#include "GameManager.h"
#include "Window.h"
#include "TestObjects.h"

int main()
{
    Window& window = Window::Instance();
	GameManager& gameManager = GameManager::Instance();
	window.WindowLoop();
	gameManager.Exit();
}