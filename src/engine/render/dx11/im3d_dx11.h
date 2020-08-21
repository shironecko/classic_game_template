#pragma once

namespace cgt::render
{

class Camera;

class Im3dDx11
{
public:
    Im3dDx11(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);

    void Render(const Camera& camera, u32 viewportWidth, u32 viewportHeight);

private:
    struct ShaderSet
    {
        ComPtr<ID3D11VertexShader> vs;
        ComPtr<ID3D11GeometryShader> gs;
        ComPtr<ID3D11PixelShader> ps;
    };

    ComPtr<ID3D11Device> m_Device;
    ComPtr<ID3D11DeviceContext> m_Context;

    ShaderSet m_Shaders[Im3d::DrawPrimitive_Count];
    ComPtr<ID3D11InputLayout> m_InputLayout;
    ComPtr<ID3D11RasterizerState> m_RasterizerState;
    ComPtr<ID3D11BlendState> m_BlendState;
    ComPtr<ID3D11DepthStencilState> m_DepthStencilState;

    ComPtr<ID3D11Buffer> m_ConstantBuffer;
    ComPtr<ID3D11Buffer> m_VertexBuffer;
    u32 m_VertexBufferSize;
};

}