#pragma once

#include "Core/Application.h"

extern Brickview::Application* Brickview::createApplication();

int main()
{
    Brickview::Log::init();

    Brickview::Application* app = Brickview::createApplication();
    app->run();
    delete app;

    return 0;
}