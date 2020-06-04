#include <engine/pch.h>

#include <engine/window.h>

namespace cgt
{

WindowConfig::WindowConfig()
{
    // some hopefully sensible defaults
    title = "Classic Game Template";
    width = 1280;
    height = 720;
    resizable = false;
}

std::shared_ptr<Window> Window::BuildWithConfig(const WindowConfig& config)
{
    auto window = SDL_CreateWindow(
        config.title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.width,
        config.height,
        SDL_WINDOW_SHOWN);

    ImGui::CreateContext();

    // the fuck? when did standard SDL2 bindings has started requiring specification of a concrete render backend?
    ImGui_ImplSDL2_InitForD3D(window);

    Window* windowWrapper = new Window(window);
    return std::shared_ptr<Window>(windowWrapper);
}

Window::Window(SDL_Window* window)
    : m_Window(window)
{
}

Window::~Window()
{
    ImGui_ImplSDL2_Shutdown();
    SDL_DestroyWindow(m_Window);
}

bool Window::PollEvent(SDL_Event& outEvent)
{
    return SDL_PollEvent(&outEvent) == 1;
}

u32 Window::GetWidth() const
{
    int width, unused;
    SDL_GL_GetDrawableSize(m_Window, &width, &unused);

    return width;
}

u32 Window::GetHeight() const
{
    int unused, height;
    SDL_GL_GetDrawableSize(m_Window, &unused, &height);

    return height;
}

void Window::NewFrame()
{
    ImGui_ImplSDL2_NewFrame(m_Window);
    ImGui::NewFrame();
}
}
