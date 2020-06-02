#include <render_core/pch.h>

#include <render_core/render_config.h>

namespace cgt::render
{

RenderConfig::RenderConfig(std::shared_ptr<Window> window)
    : m_Window(std::move(window))
{
}

}
