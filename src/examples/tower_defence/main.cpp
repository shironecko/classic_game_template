#include <examples/tower_defence/pch.h>

int GameMain()
{
    auto window = cgt::WindowConfig::Default()
        .WithTitle("Tower Defence")
        .Build();

    auto render = cgt::render::RenderConfig::Default(window)
        .Build();

    cgt::render::CameraSimpleOrtho camera(*window);
    camera.pixelsPerUnit = 32.0f;

    cgt::Clock clock;
    cgt::render::RenderQueue renderQueue;
    SDL_Event event {};

    bool quitRequested = false;
    while (!quitRequested)
    {
        const float dt = clock.Tick();

        render->NewFrame();
        window->NewFrame();

        while (window->PollEvent(event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quitRequested = true;
                break;
            }
        }

        renderQueue.Reset();
        renderQueue.clearColor = { 0.5f, 0.5f, 0.5f, 1.0f };

        render->Submit(renderQueue, camera);
    }

    return 0;
}