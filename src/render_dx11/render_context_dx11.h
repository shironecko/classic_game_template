#pragma once

#include <engine/window.h>
#include <render_core/i_render_context.h>
#include <render_core/i_camera.h>
#include <DirectXTK/CommonStates.h>

namespace cgt::render
{

class Im3dDx11;

class TextureData : private NonCopyable
{
private:
    friend class RenderContextDX11;

    TextureData() = default;

    ComPtr<ID3D11ShaderResourceView> m_View;
};

class RenderContextDX11 : public IRenderContext, private NonCopyable
{
public:
    static std::unique_ptr<RenderContextDX11> Create(SDL_Window* window);

    TextureHandle LoadTexture(const std::filesystem::path& absolutePath) override;
    ImTextureID GetImTextureID(const TextureHandle& texture) override;
    usize GetTextureSortKey(const TextureHandle& texture) override;

    void Clear(glm::vec4 clearColor, glm::uvec2 windowDimensions) override;
    RenderStats Submit(SpriteDrawList& drawList, const ICamera& camera, glm::uvec2 windowDimensions, bool sortBeforeRendering = true) override;
    void Present() override;

protected:
    void ImGuiBindingsInit() override;
    void ImGuiBindingsNewFrame() override;
    void ImGuiBindingsRender(ImDrawData* drawData) override;
    void ImGuiBindingsShutdown() override;

    void Im3dBindingsInit() override;
    void Im3dBindingsNewFrame() override;
    void Im3dBindingsRender(const ICamera& camera, glm::uvec2 windowDimensions) override;
    void Im3dBindingsShutdown() override;

private:
    static constexpr usize MAX_BATCH_SIZE = 1024;

    RenderContextDX11() = default;

    void SetUpRenderTarget(glm::uvec2 windowDimensions);
    HRESULT LoadTextureFromMemory(const u8* data, usize size, TextureData& outData);

    ComPtr<ID3D11Device> m_Device;
    ComPtr<ID3D11DeviceContext> m_Context;
    ComPtr<IDXGISwapChain> m_Swapchain;
    ComPtr<ID3D11RenderTargetView> m_RTView;

    std::unique_ptr<DirectX::CommonStates> m_CommonStates;

    ComPtr<ID3D11VertexShader> m_VertexShader;
    ComPtr<ID3D11PixelShader> m_PixelShader;
    ComPtr<ID3D11InputLayout> m_InputLayout;
    ComPtr<ID3D11Buffer> m_FrameConstants;

    ComPtr<ID3D11Buffer> m_QuadVertices;
    ComPtr<ID3D11Buffer> m_QuadUV;
    ComPtr<ID3D11Buffer> m_QuadIndices;

    ComPtr<ID3D11Buffer> m_SpriteInstanceData;

    TextureData m_MissingTexture;

    std::unique_ptr<Im3dDx11> m_Im3dRender;
};

}
