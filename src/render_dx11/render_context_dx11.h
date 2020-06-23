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

    ComPtr<ID3D11Resource> m_Resource;
    ComPtr<ID3D11ShaderResourceView> m_View;
};

class RenderContextDX11 : public IRenderContext, private NonCopyable
{
public:
    static std::shared_ptr<RenderContextDX11> BuildWithConfig(RenderConfig config);

    TextureHandle LoadTexture(const std::filesystem::path& absolutePath) override;

    RenderStats Submit(RenderQueue& queue, const ICamera& camera) override;
    void Present() override;

protected:
    void ImGuiBindingsInit() override;
    void ImGuiBindingsNewFrame() override;
    void ImGuiBindingsRender(ImDrawData* drawData) override;
    void ImGuiBindingsShutdown() override;

    void Im3dBindingsInit() override;
    void Im3dBindingsNewFrame() override;
    void Im3dBindingsRender(const ICamera& camera) override;
    void Im3dBindingsShutdown() override;

private:
    static constexpr usize MAX_BATCH_SIZE = 1024;

    explicit RenderContextDX11(std::shared_ptr<Window> window);

    HRESULT LoadTextureFromMemory(const u8* data, usize size, TextureData& outData);

    std::shared_ptr<Window> m_Window;

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
