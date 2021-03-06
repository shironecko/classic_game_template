#pragma once

#include <engine/render/core/sprite_draw_list.h>
#include <engine/render/core/camera.h>

namespace cgt
{
class UIBindings;
}

namespace cgt::render
{

struct RenderStats
{
    void Reset() { spriteCount = 0; drawcallCount = 0; }

    u32 spriteCount = 0;
    u32 drawcallCount = 0;
};

class IRenderContext
{
public:
    // meant to be implemented by concrete rendering libraries
    static std::unique_ptr<IRenderContext> Create(SDL_Window* window);

    virtual TextureHandle LoadTexture(const std::filesystem::path& absolutePath) = 0;
    virtual ImTextureID GetImTextureID(const TextureHandle& texture) = 0;
    virtual usize GetTextureSortKey(const TextureHandle& texture) = 0;

    virtual void Clear(glm::vec4 clearColor, glm::uvec2 windowDimensions) = 0;
    virtual void Submit(SpriteDrawList& drawList, const Camera& camera, glm::uvec2 windowDimensions, bool sortBeforeRendering = true) = 0;
    virtual RenderStats Present() = 0;

    virtual ~IRenderContext() = default;

protected:
    friend class ::cgt::UIBindings;

    virtual void ImGuiBindingsInit(SDL_Window* window) = 0;
    virtual void ImGuiBindingsNewFrame() = 0;
    virtual void ImGuiBindingsRender(ImDrawData* drawData) = 0;
    virtual void ImGuiBindingsShutdown() = 0;

    virtual void Im3dBindingsInit() = 0;
    virtual void Im3dBindingsNewFrame() = 0;
    virtual void Im3dBindingsRender(const Camera& camera, glm::uvec2 windowDimensions) = 0;
    virtual void Im3dBindingsShutdown() = 0;
};

}
