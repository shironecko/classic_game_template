#pragma once

namespace cgt
{

class Window;

namespace render
{
    class IRenderContext;
}

class ImGuiHelper
{
public:
    static std::unique_ptr<ImGuiHelper> Create(std::shared_ptr<Window> window, std::shared_ptr<render::IRenderContext> render);

    ~ImGuiHelper();

    void NewFrame(float dt);
    void RenderUi();

private:
    ImGuiHelper(std::shared_ptr<Window> window, std::shared_ptr<render::IRenderContext> render);

    std::shared_ptr<Window> m_Window;
    std::shared_ptr<render::IRenderContext> m_Render;
};

}