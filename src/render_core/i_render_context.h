#pragma once

#include <memory>
#include <render_core/render_config.h>
#include <render_core/render_queue.h>

namespace cgt::render
{

struct RenderStats
{
    usize spriteCount;
    usize drawcallCount;
};

class IRenderContext
{
public:
    // meant to be implemented by concrete rendering libraries
    static std::shared_ptr<IRenderContext> BuildWithConfig(RenderConfig config);

    virtual RenderStats Submit(RenderQueue& queue) = 0;
    virtual TextureHandle LoadTexture(const char* path) = 0;

    virtual ~IRenderContext() = default;
};

}
