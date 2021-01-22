#include "Renderer.h"

int main()
{
    Renderer& renderer = Renderer::Instance();
    renderer.Initialise();
    renderer.WindowLoop();
}