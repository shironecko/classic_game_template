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

    virtual TextureHandle LoadTexture(const std::filesystem::path& absolutePath) = 0;

    virtual RenderStats Submit(RenderQueue& queue, const ICamera& camera) = 0;
    virtual void Present() = 0;

    virtual ~IRenderContext() = default;

protected:
    friend class ImGuiHelper;

    virtual void ImGuiBindingsInit() = 0;
    virtual void ImGuiBindingsNewFrame() = 0;
    virtual void ImGuiBindingsRender(ImDrawData* drawData) = 0;
    virtual void ImGuiBindingsShutdown() = 0;

    virtual void Im3dBindingsInit() = 0;
    virtual void Im3dBindingsNewFrame() = 0;
    virtual void Im3dBindingsRender(const ICamera& camera) = 0;
    virtual void Im3dBindingsShutdown() = 0;
};

}
