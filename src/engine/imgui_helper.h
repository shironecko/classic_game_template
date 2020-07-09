#pragma once

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

class ImGuiHelper
{
public:
    static std::unique_ptr<ImGuiHelper> Create(std::shared_ptr<Window> window, std::shared_ptr<render::IRenderContext> render);

    ~ImGuiHelper();

    void NewFrame(float dt, const render::ICamera& camera);
    void RenderUi(const render::ICamera& camera);

    void BeginInvisibleFullscreenWindow();
    void EndInvisibleFullscreenWindow();

private:
    ImGuiHelper(std::shared_ptr<Window> window, std::shared_ptr<render::IRenderContext> render);

    void RenderIm3dText(const render::ICamera& camera);

    std::shared_ptr<Window> m_Window;
    std::shared_ptr<render::IRenderContext> m_Render;
};

}