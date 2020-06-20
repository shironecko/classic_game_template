#include <engine/pch.h>

#include <engine/imgui_helper.h>
#include <engine/window.h>
#include <render_core/i_render_context.h>

namespace cgt
{

std::unique_ptr<ImGuiHelper> ImGuiHelper::Create(
    std::shared_ptr<Window> window,
    std::shared_ptr<render::IRenderContext> render
)
{
    auto helper = std::unique_ptr<ImGuiHelper>(new ImGuiHelper(window, render));
    return helper;
}

ImGuiHelper::ImGuiHelper(std::shared_ptr<Window> window, std::shared_ptr<render::IRenderContext> render)
    : m_Window(std::move(window))
    , m_Render(std::move(render))
{
    ImGui::CreateContext();

    // TODO: is this the best way to do this?
#ifdef WIN32
    ImGui_ImplSDL2_InitForD3D(m_Window->GetSDLWindow());
#else
#error "Platform not supported!"
#endif

    m_Render->ImGuiBindingsInit();
}

ImGuiHelper::~ImGuiHelper()
{
    m_Render->ImGuiBindingsShutdown();

    ImGui_ImplSDL2_Shutdown();
}

void ImGuiHelper::NewFrame(float dt)
{
    m_Render->ImGuiBindingsNewFrame();
    ImGui_ImplSDL2_NewFrame(m_Window->GetSDLWindow());
    ImGui::NewFrame();
}

void ImGuiHelper::RenderUi()
{
    ImGui::Render();
    m_Render->ImGuiBindingsRender(ImGui::GetDrawData());
}

}
