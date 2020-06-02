#pragma once

#include <engine/window.h>
#include <render_core/i_render_context.h>
#include <DirectXTK/CommonStates.h>

namespace cgt::render
{

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

    RenderStats Submit(RenderQueue& queue) override;
    TextureHandle LoadTexture(const char* path) override;

    ~RenderContextDX11() override;

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

    ComPtr<ID3D11Buffer> m_InstanceWorldTransform;
    ComPtr<ID3D11Buffer> m_InstanceUVTransform;
    ComPtr<ID3D11Buffer> m_InstanceColor;

    TextureData m_MissingTexture;
};

}
