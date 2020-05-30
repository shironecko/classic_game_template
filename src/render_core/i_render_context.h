#pragma once

#include <memory>
#include <render_core/render_config.h>
#include <render_core/render_queue.h>

namespace cgt::render
{

class IRenderContext
{
public:
    // meant to be implemented by concrete rendering libraries
    static std::shared_ptr<IRenderContext> BuildWithConfig(RenderConfig config);

    virtual void Submit(RenderQueue& queue) = 0;

    virtual ~IRenderContext() = default;
};

}
