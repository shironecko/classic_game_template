#pragma once

#include <engine/window.h>

namespace cgt::render
{

class IRenderContext;

class RenderConfig
{
public:
    static RenderConfig Default(std::shared_ptr<Window> window);
    explicit RenderConfig(std::shared_ptr<Window> window);

    std::shared_ptr<IRenderContext> Build();

    std::shared_ptr<Window> GetSDLWindow() { return m_Window; }

private:
    std::shared_ptr<Window> m_Window;
};

}
