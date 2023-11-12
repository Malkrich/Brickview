#include "Core\Application\Application.h"
#include "Core\Log.h"

int main()
{
    Brickview::Log::init();

    Brickview::Application* app = new Brickview::Application();

    app->run();

    delete app;

    return 0;
}