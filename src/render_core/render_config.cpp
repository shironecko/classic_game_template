#include <render_core/pch.h>

#include <render_core/render_config.h>
#include <render_core/i_render_context.h>

namespace cgt::render
{

RenderConfig RenderConfig::Default(std::shared_ptr<Window> window)
{
    return RenderConfig(window);
}

RenderConfig::RenderConfig(std::shared_ptr<Window> window)
    : m_Window(std::move(window))
{
}

std::shared_ptr<IRenderContext> RenderConfig::Build()
{
    return IRenderContext::BuildWithConfig(*this);
}

}
