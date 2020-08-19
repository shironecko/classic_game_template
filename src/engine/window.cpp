#include <engine/pch.h>

#include <engine/window.h>

namespace cgt
{

Window::Window(const char* title, glm::uvec2 dimensions)
{
    m_Window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        dimensions.x,
        dimensions.y,
        SDL_WINDOW_SHOWN);
}

Window::~Window()
{
    SDL_DestroyWindow(m_Window);
}

bool Window::PollEvent(SDL_Event& outEvent)
{
    return SDL_PollEvent(&outEvent) == 1;
}

glm::uvec2 Window::GetDimensions() const
{
    int width, height;
    SDL_GL_GetDrawableSize(m_Window, &width, &height);

    return { (u32)width, (u32)height };
}

}
