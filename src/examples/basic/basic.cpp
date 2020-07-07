#include <engine/api.h>
#include <render_core/api.h>

int GameMain()
{
    auto window = cgt::WindowConfig::Default()
        .WithTitle("Basic Example")
        .Build();

    auto render= cgt::render::RenderConfig::Default(window)
        .Build();

    auto imguiHelper = cgt::ImGuiHelper::Create(window, render);

    cgt::render::SpriteDrawList drawList;
    cgt::render::CameraSimpleOrtho camera(*window);
    camera.pixelsPerUnit = 16.0f;

    cgt::Clock frameClock;
    SDL_Event event {};
    cgt::render::RenderStats renderStats {};
    bool quitRequested = false;
    while (!quitRequested)
    {
        const float dt = frameClock.Tick();

        imguiHelper->NewFrame(dt, camera);

        while (window->PollEvent(event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quitRequested = true;
                break;
            }
        }

        {
            ImGui::SetNextWindowSize({200, 80}, ImGuiCond_FirstUseEver);
            ImGui::Begin("Render Stats");
            ImGui::Text("Frame time: %.2fms", dt * 1000.0f);
            ImGui::Text("Sprites: %d", renderStats.spriteCount);
            ImGui::Text("Drawcalls: %d", renderStats.drawcallCount);
            ImGui::End();
        }

        drawList.clear();
        auto& testSprite = drawList.AddSprite();
        testSprite.scale = {10.0f, 10.0f};

        render->Clear({ 1.0f, 0.3f, 1.0f, 1.0f });
        renderStats = render->Submit(drawList, camera);
        imguiHelper->RenderUi(camera);
        render->Present();
    }

    return 0;
}