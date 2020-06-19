#pragma once

#include <memory>
#include <render_core/render_config.h>
#include <render_core/render_queue.h>
#include <render_core/i_camera.h>

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

    virtual RenderStats Submit(RenderQueue& queue, const ICamera& camera) = 0;
    virtual TextureHandle LoadTexture(const std::filesystem::path& absolutePath) = 0;

    virtual void NewFrame() = 0;

    virtual ~IRenderContext() = default;
};

}
