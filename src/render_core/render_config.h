#pragma once

#include <engine/window.h>

namespace cgt::render
{
class RenderConfig
{
public:
    explicit RenderConfig(std::shared_ptr<Window> window);

    std::shared_ptr<Window> GetSDLWindow() { return m_Window; }

private:
    std::shared_ptr<Window> m_Window;
};

}
