#pragma once

namespace cgt
{

class Window : private NonCopyable
{
public:
    Window(const char* title, glm::uvec2 dimensions);
    ~Window();

    glm::uvec2 GetDimensions() const;
    SDL_Window* GetSDLWindow() { return m_Window; }
    bool PollEvent(SDL_Event& outEvent);

private:
    SDL_Window* m_Window;
};

}
