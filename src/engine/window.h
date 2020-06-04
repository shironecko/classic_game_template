#pragma once

namespace cgt
{

class WindowConfig
{
public:
    WindowConfig();

    const char* title;
    u32 width;
    u32 height;
    bool resizable;
};

class Window : private NonCopyable
{
public:
    static std::shared_ptr<Window> BuildWithConfig(const WindowConfig& config);

    ~Window();

    bool PollEvent(SDL_Event& outEvent);

    u32 GetWidth() const;
    u32 GetHeight() const;

    void NewFrame();

    SDL_Window* GetSDLWindow() { return m_Window; }

private:
    explicit Window(SDL_Window* window);

    SDL_Window* m_Window;
};

}
