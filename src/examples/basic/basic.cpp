#include <engine/system.h>
#include <engine/external_libs.h>
#include <engine/window.h>
#include <engine/assets.h>

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
    camera.pixelsPerUnit = 512.0f;

    auto tiledMap = cgt::LoadTiledMap("assets/examples/maps/sample_map.tmx");

    SDL_Event event {};
    bool quitRequested = false;
    while (!quitRequested)
    {
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
        renderQueue.clearColor = glm::vec4(1.0f, 0.3f, 1.0f, 1.0f);

        cgt::render::SpriteDrawRequest sprite {};
        //sprite.texture = atlasTexture;
        renderQueue.sprites.emplace_back(sprite);

        render->Submit(renderQueue, camera);
    }

    return 0;
}