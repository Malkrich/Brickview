#pragma once

#include "Core/Application.h"
#include "Core/Log.h"

extern Brickview::Application* Brickview::createApplication();

int main()
{
    Brickview::Log::init();

    Brickview::Application* app = Brickview::createApplication();

    app->run();

    delete app;

    return 0;
}