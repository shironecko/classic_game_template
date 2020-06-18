#include <engine/system.h>
#include <engine/external_libs.h>
#include <engine/window.h>
#include <engine/assets.h>
#include <engine/tilemap.h>
#include <engine/clock.h>

#include <render_core/i_render_context.h>
#include <render_core/camera_simple_ortho.h>

int GameMain()
{
    auto windowCfg = cgt::WindowConfig();
    windowCfg.title = "Basic Example";
    auto window = cgt::Window::BuildWithConfig(windowCfg);

    auto renderCfg = cgt::render::RenderConfig(window);
    auto render = cgt::render::IRenderContext::BuildWithConfig(renderCfg);

    cgt::render::RenderQueue renderQueue;
    cgt::render::CameraSimpleOrtho camera;
    camera.windowWidth = window->GetWidth();
    camera.windowHeight = window->GetHeight();
    camera.pixelsPerUnit = 16.0f;

    cgt::Clock frameClock;
    SDL_Event event {};
    cgt::render::RenderStats renderStats{};
    bool quitRequested = false;
    while (!quitRequested)
    {
        const float dt = frameClock.Tick();

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

        {
            ImGui::SetNextWindowSize({200, 80}, ImGuiCond_FirstUseEver);
            ImGui::Begin("Render Stats");
            ImGui::Text("Frame time: %.2fms", dt * 1000.0f);
            ImGui::Text("Sprites: %d", renderStats.spriteCount);
            ImGui::Text("Drawcalls: %d", renderStats.drawcallCount);
            ImGui::End();
        }

        renderQueue.Reset();
        renderQueue.clearColor = glm::vec4(1.0f, 0.3f, 1.0f, 1.0f);
        cgt::render::SpriteDrawRequest testSprite;
        testSprite.scale = {10.0f, 10.0f};
        renderQueue.sprites.emplace_back(testSprite);

        renderStats = render->Submit(renderQueue, camera);
    }

    return 0;
}