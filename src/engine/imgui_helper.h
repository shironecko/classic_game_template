#pragma once

#include <engine/event_loop.h>

namespace cgt
{

class Window;

namespace render
{
class ICamera;
}

namespace render
{
    class IRenderContext;
}

class ImGuiHelper : public IEventListener
{
public:
    ImGuiHelper(Window& window, render::IRenderContext& render);
    void Shutdown(render::IRenderContext& render);

    void NewFrame(Window& window, render::IRenderContext& render, const render::ICamera& camera, float deltaTime);
    void RenderUi(render::IRenderContext& render, const render::ICamera& camera, glm::uvec2 windowDimensions);

    static void BeginInvisibleFullscreenWindow();
    static void EndInvisibleFullscreenWindow();

    IEventListener::EventAction OnEvent(const SDL_Event& event) override;

private:
    void RenderIm3dText(const render::ICamera& camera);

    glm::uvec2 m_WindowDimensions;
};

}