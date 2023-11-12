#include "Core\Application\Application.h"

int main()
{
    Brickview::Application* app = new Brickview::Application();

    app->run();

    delete app;

    return 0;
}