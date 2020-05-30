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

    Window* windowWrapper = new Window(window);
    return std::shared_ptr<Window>(windowWrapper);
}

Window::Window(SDL_Window* window)
    : m_Window(window)
{
}

Window::~Window()
{
    SDL_DestroyWindow(m_Window);
}

bool Window::PollEvent(SDL_Event& outEvent)
{
    return SDL_PollEvent(&outEvent) == 1;
}

}
