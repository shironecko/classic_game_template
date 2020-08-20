#pragma once

#include <engine/window.h>

namespace cgt
{

namespace render
{
class IRenderContext;
class ICamera;
}

class UIBindings final
{
public:
    UIBindings(Window& window, render::IRenderContext& render);
    void Shutdown(render::IRenderContext& render);

    void NewFrame(Window& window, render::IRenderContext& render, const render::ICamera& camera, float deltaTime);
    void RenderUi(Window& window, render::IRenderContext& render, const render::ICamera& camera);

    WindowEventControlFlow ProcessWindowEvent(const SDL_Event& event);

private:
    void RenderIm3dText(const render::ICamera& camera);
};

}