#include <engine/render/dx11/pch.h>

#include <engine/render/dx11/im3d_dx11.h>
#include <engine/render/dx11/util.h>

namespace cgt::render
{
Im3dDx11::Im3dDx11(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
    : m_Device(std::move(device))
    , m_Context(std::move(context))
    , m_VertexBufferSize(0)
{
    const char* shaderTypes[] = { "VERTEX_SHADER", "GEOMETRY_SHADER", "PIXEL_SHADER", };
    const char* primitiveTypes[Im3d::DrawPrimitive_Count];
    primitiveTypes[Im3d::DrawPrimitive_Points] = "POINTS";
    primitiveTypes[Im3d::DrawPrimitive_Lines] = "LINES";
    primitiveTypes[Im3d::DrawPrimitive_Triangles] = "TRIANGLES";

    auto shaderPath = AssetPath("engine/shaders/dx11/im3d.hlsl");
    const char* entryPoint = "main";
    for (u32 primitiveIdx = 0; primitiveIdx < Im3d::DrawPrimitive_Count; ++primitiveIdx)
    {
        ShaderSet& set = m_Shaders[primitiveIdx];
        const char* primitiveType = primitiveTypes[primitiveIdx];

        {
            const char* shaderType = shaderTypes[0]; // vertex
            D3D_SHADER_MACRO macro[] = { shaderType, "", primitiveType, "", nullptr, nullptr };
            auto vsData = CompileShader(shaderPath, entryPoint, "vs_4_0", macro);

            HRESULT hresult = m_Device->CreateVertexShader(
                vsData->GetBufferPointer(),
                vsData->GetBufferSize(),
                nullptr,
                set.vs.GetAddressOf());
            CGT_CHECK_HRESULT(hresult, "Failed to compile {} for {} primitive!", shaderType, primitiveType);

            if (primitiveIdx == 0)
            {
                D3D11_INPUT_ELEMENT_DESC desc[] = {
                    {
                        "POSITION_SIZE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
                        (UINT)offsetof(Im3d::VertexData, m_positionSize), D3D11_INPUT_PER_VERTEX_DATA, 0
                    },
                    {
                        "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)offsetof(Im3d::VertexData, m_color),
                        D3D11_INPUT_PER_VERTEX_DATA, 0
                    },
                };
                hresult = m_Device->CreateInputLayout(
                    desc,
                    CGT_ARRAY_LENGTH(desc),
                    vsData->GetBufferPointer(),
                    vsData->GetBufferSize(),
                    m_InputLayout.GetAddressOf());
                CGT_CHECK_HRESULT(hresult, "Failed to create input layout!");
            }
        }

        // triangles do not need geometry shader
        if (primitiveIdx != Im3d::DrawPrimitive_Triangles)
        {
            const char* shaderType = shaderTypes[1]; // geometry
            D3D_SHADER_MACRO macro[] = { shaderType, "", primitiveType, "", nullptr, nullptr };
            auto vsData = CompileShader(shaderPath, entryPoint, "gs_4_0", macro);

            HRESULT hresult = m_Device->CreateGeometryShader(
                vsData->GetBufferPointer(),
                vsData->GetBufferSize(),
                nullptr,
                set.gs.GetAddressOf());
            CGT_CHECK_HRESULT(hresult, "Failed to compile {} for {} primitive!", shaderType, primitiveType);
        }

        {
            const char* shaderType = shaderTypes[2]; // pixel
            D3D_SHADER_MACRO macro[] = { shaderType, "", primitiveType, "", nullptr, nullptr };
            auto vsData = CompileShader(shaderPath, entryPoint, "ps_4_0", macro);

            HRESULT hresult = m_Device->CreatePixelShader(
                vsData->GetBufferPointer(),
                vsData->GetBufferSize(),
                nullptr,
                set.ps.GetAddressOf());
            CGT_CHECK_HRESULT(hresult, "Failed to compile {} for {} primitive!", shaderType, primitiveType);
        }
    }

    {
        D3D11_RASTERIZER_DESC desc {};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        HRESULT hresult = m_Device->CreateRasterizerState(&desc, m_RasterizerState.GetAddressOf());
        CGT_CHECK_HRESULT(hresult, "Failed to create rasterizer state!");
    }

    {
        D3D11_DEPTH_STENCIL_DESC desc {};
        HRESULT hresult = m_Device->CreateDepthStencilState(&desc, m_DepthStencilState.GetAddressOf());
        CGT_CHECK_HRESULT(hresult, "Failed to create depth stencil state!");
    }

    {
        D3D11_BLEND_DESC desc {};
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        HRESULT hresult = m_Device->CreateBlendState(&desc, m_BlendState.GetAddressOf());
        CGT_CHECK_HRESULT(hresult, "Failed to create blend state!");
    }

    m_ConstantBuffer = CreateBuffer(
        m_Device.Get(),
        nullptr,
        sizeof(Im3d::Mat4) + sizeof(Im3d::Vec4),
        D3D11_BIND_CONSTANT_BUFFER,
        D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_WRITE);
}

void Im3dDx11::Render(const Camera& camera, u32 viewportWidth, u32 viewportHeight)
{
    Im3d::EndFrame();

    Im3d::AppData& ad = Im3d::GetAppData();

    D3D11_VIEWPORT viewport = {
        0.0f, 0.0f,                                             // TopLeftX, TopLeftY
        (float)viewportWidth, (float)viewportHeight, 0.0f, 1.0f // MinDepth, MaxDepth
    };
    m_Context->RSSetViewports(1, &viewport);
    m_Context->OMSetBlendState(m_BlendState.Get(), nullptr, 0xffffffff);
    m_Context->OMSetDepthStencilState(m_DepthStencilState.Get(), 0);
    m_Context->RSSetState(m_RasterizerState.Get());

    for (u32 i = 0, n = Im3d::GetDrawListCount(); i < n; ++i)
    {
        auto& drawList = Im3d::GetDrawLists()[i];

        struct Layout
        {
            Im3d::Mat4 viewProj;
            Im3d::Vec2 viewport;
        };
        Layout layout;
        layout.viewProj = camera.GetViewProjection();
        layout.viewport = ad.m_viewportSize;
        UpdateBuffer(m_Context.Get(), m_ConstantBuffer.Get(), layout);

        if (!m_VertexBuffer.Get() || m_VertexBufferSize < drawList.m_vertexCount)
        {
            m_VertexBufferSize = drawList.m_vertexCount;
            m_VertexBuffer = CreateBuffer(
                m_Device.Get(),
                nullptr,
                m_VertexBufferSize * sizeof(Im3d::VertexData),
                D3D11_BIND_VERTEX_BUFFER,
                D3D11_USAGE_DYNAMIC,
                D3D11_CPU_ACCESS_WRITE);
        }

        UpdateBuffer(
            m_Context.Get(),
            m_VertexBuffer.Get(),
            drawList.m_vertexData,
            drawList.m_vertexCount * sizeof(Im3d::VertexData));

        switch (drawList.m_primType)
        {
        case Im3d::DrawPrimitive_Points:
            m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
            break;
        case Im3d::DrawPrimitive_Lines:
            m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            break;
        case Im3d::DrawPrimitive_Triangles:
            m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            break;
        default: IM3D_ASSERT(false);
            return;
        };

        ShaderSet& shaders = m_Shaders[drawList.m_primType];
        m_Context->VSSetShader(shaders.vs.Get(), nullptr, 0);
        m_Context->GSSetShader(shaders.gs.Get(), nullptr, 0);
        m_Context->PSSetShader(shaders.ps.Get(), nullptr, 0);

        m_Context->VSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());
        m_Context->GSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());

        UINT stride = sizeof(Im3d::VertexData);
        UINT offset = 0;
        m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
        m_Context->IASetInputLayout(m_InputLayout.Get());
        m_Context->Draw(drawList.m_vertexCount, 0);
    }

    m_Context->VSSetShader(nullptr, nullptr, 0);
    m_Context->GSSetShader(nullptr, nullptr, 0);
    m_Context->PSSetShader(nullptr, nullptr, 0);
}
}
