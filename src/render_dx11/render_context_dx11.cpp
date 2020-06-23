#include <render_dx11/pch.h>

#include <render_dx11/render_context_dx11.h>
#include <render_dx11/im3d_dx11.h>
#include <render_dx11/util.h>
#include <engine/assets.h>

#include <SDL2/SDL_syswm.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/DirectXHelpers.h>

namespace cgt::render
{

struct SpriteInstanceData
{
    glm::vec4 colorTint;
    glm::vec2 position;
    glm::vec2 uvMin;
    glm::vec2 uvMax;
    glm::vec2 scale;
    float rotation;
    float depth;
};

std::shared_ptr<IRenderContext> IRenderContext::BuildWithConfig(RenderConfig config)
{
    return RenderContextDX11::BuildWithConfig(std::move(config));
}

std::shared_ptr<RenderContextDX11> RenderContextDX11::BuildWithConfig(RenderConfig config)
{
    auto context = std::shared_ptr<RenderContextDX11>(new RenderContextDX11(config.GetSDLWindow()));

    UINT deviceFlags = 0;
    CGT_DEBUG_ONLY(deviceFlags |= D3D11_CREATE_DEVICE_DEBUG);

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hresult = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        context->m_Device.GetAddressOf(),
        &featureLevel,
        context->m_Context.GetAddressOf());

    CGT_CHECK_HRESULT(hresult, "Failed to create D3D11 Device and Context!");
    CGT_ASSERT_ALWAYS_MSG(featureLevel == D3D_FEATURE_LEVEL_11_0, "D3D Feature Level 11 is not supported!");

    hresult = context->LoadTextureFromMemory(MISSINGNO_PNG, sizeof(MISSINGNO_PNG), context->m_MissingTexture);
    CGT_CHECK_HRESULT(hresult, "Failed to create a stub missing texture!");

    context->m_CommonStates = std::make_unique<DirectX::CommonStates>(context->m_Device.Get());

    ComPtr<IDXGIDevice> dxgiDevice;
    hresult = context->m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)dxgiDevice.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Couldn't query for IDXGIDevice!");

    ComPtr<IDXGIAdapter> dxgiAdapter;
    hresult = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)dxgiAdapter.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Couldn't query for IDXGIAdapter!");

    ComPtr<IDXGIFactory> dxgiFactory;
    hresult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)dxgiFactory.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Couldn't query for IDXGIFactory!");

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(context->m_Window->GetSDLWindow(), &wmInfo);
    HWND hwnd = wmInfo.info.win.window;

    DXGI_MODE_DESC modeDesc {};
    modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    modeDesc.Width = context->m_Window->GetWidth();
    modeDesc.Height = context->m_Window->GetHeight();
    modeDesc.RefreshRate.Numerator = 60;
    modeDesc.RefreshRate.Denominator = 1;

    DXGI_SWAP_CHAIN_DESC swapDesc {};
    swapDesc.BufferDesc = modeDesc;
    swapDesc.SampleDesc = DXGI_SAMPLE_DESC { 1, 0 };
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapDesc.Windowed = true;
    swapDesc.OutputWindow = hwnd;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount = 1;

    hresult = dxgiFactory->CreateSwapChain(context->m_Device.Get(), &swapDesc, context->m_Swapchain.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create a swapchain!");

    ComPtr<ID3D11Texture2D> backBuffer;
    hresult = context->m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to get swapchain back buffer!");

    hresult = context->m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, context->m_RTView.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create render target view!");

    ComPtr<ID3D10Blob> vertexShaderBlob = CompileShader(
        AssetPath("engine/shaders/dx11/sprites.hlsl"),
        "VSMain",
        "vs_4_0",
        nullptr);
    hresult = context->m_Device->CreateVertexShader(
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        context->m_VertexShader.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create vertex shader from bytecode!");
    DirectX::SetDebugObjectName(context->m_VertexShader.Get(), "Sprite VS");

    ComPtr<ID3D10Blob> pixelShaderBlob = CompileShader(
        AssetPath("engine/shaders/dx11/sprites.hlsl"),
        "PSMain",
        "ps_4_0",
        nullptr);
    hresult = context->m_Device->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        context->m_PixelShader.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create pixel shader from bytecode!");
    DirectX::SetDebugObjectName(context->m_PixelShader.Get(), "Sprite PS");

    const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            // per-vertex
            { "QUAD_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            // per-instance
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, offsetof(SpriteInstanceData, colorTint), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "SPRITE_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 2, offsetof(SpriteInstanceData, position), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TEXCOORD_MIN", 0, DXGI_FORMAT_R32G32_FLOAT, 2, offsetof(SpriteInstanceData, uvMin), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TEXCOORD_MAX", 0, DXGI_FORMAT_R32G32_FLOAT, 2, offsetof(SpriteInstanceData, uvMax), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 2, offsetof(SpriteInstanceData, scale), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 2, offsetof(SpriteInstanceData, rotation), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "DEPTH", 0, DXGI_FORMAT_R32_FLOAT, 2, offsetof(SpriteInstanceData, depth), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };

    hresult = context->m_Device->CreateInputLayout(
        inputElementDesc,
        SDL_arraysize(inputElementDesc),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        context->m_InputLayout.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create shader input layout!");
    DirectX::SetDebugObjectName(context->m_InputLayout.Get(), "Sprite VS Input Layout");

    const float quadVertices[] =
        {
            -0.5f, 0.5f,
            0.5f, 0.5f,
            0.5f, -0.5f,
            -0.5f, -0.5f,
        };
    context->m_QuadVertices = CreateStaticBuffer(
        context->m_Device.Get(),
        quadVertices,
        sizeof(quadVertices),
        D3D11_BIND_VERTEX_BUFFER);
    DirectX::SetDebugObjectName(context->m_QuadVertices.Get(), "Quad Vertices");

    // centered around (0, 0) for scaling in a shader
    const float quadUVs[] =
        {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f
        };
    context->m_QuadUV = CreateStaticBuffer(
        context->m_Device.Get(),
        quadUVs,
        sizeof(quadUVs),
        D3D11_BIND_VERTEX_BUFFER);
    DirectX::SetDebugObjectName(context->m_QuadUV.Get(), "Quad UVs");

    const u16 quadIdxs[] =
        {
            0, 1, 2,
            2, 3, 0
        };
    context->m_QuadIndices = CreateStaticBuffer(
        context->m_Device.Get(),
        quadIdxs,
        sizeof(quadIdxs),
        D3D11_BIND_INDEX_BUFFER);
    DirectX::SetDebugObjectName(context->m_QuadIndices.Get(), "Quad Indices");

    context->m_SpriteInstanceData = CreateBuffer(
        context->m_Device.Get(),
        nullptr,
        sizeof(SpriteInstanceData) * MAX_BATCH_SIZE,
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_WRITE);
    DirectX::SetDebugObjectName(context->m_SpriteInstanceData.Get(), "Sprite Instance Data");

    context->m_FrameConstants = CreateBuffer(
        context->m_Device.Get(),
        nullptr,
        sizeof(glm::mat4),
        D3D11_BIND_CONSTANT_BUFFER,
        D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_WRITE);
    DirectX::SetDebugObjectName(context->m_FrameConstants.Get(), "Frame Constants");

    return context;
}

RenderStats RenderContextDX11::Submit(RenderQueue& queue, const ICamera& camera)
{
    ZoneScoped;

    RenderStats stats {};
    {
        ZoneScopedN("Setup");
        stats.spriteCount = queue.sprites.size();

        D3D11_VIEWPORT viewport {};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = (float)m_Window->GetWidth();
        viewport.Height = (float)m_Window->GetHeight();
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        m_Context->RSSetViewports(1, &viewport);
        m_Context->RSSetState(m_CommonStates->CullNone());

        m_Context->OMSetRenderTargets(1, m_RTView.GetAddressOf(), nullptr);
        m_Context->ClearRenderTargetView(m_RTView.Get(), &queue.clearColor.x);

        m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_Context->IASetIndexBuffer(m_QuadIndices.Get(), DXGI_FORMAT_R16_UINT, 0);
        m_Context->IASetInputLayout(m_InputLayout.Get());

        const UINT strides[] =
            {
                sizeof(glm::vec2), // POSITION
                sizeof(glm::vec2), // TEXCOORD
                sizeof(SpriteInstanceData),
            };
        const UINT offsets[] = { 0, 0, 0, };
        ID3D11Buffer* buffers[] =
            {
                m_QuadVertices.Get(),
                m_QuadUV.Get(),
                m_SpriteInstanceData.Get(),
            };
        m_Context->IASetVertexBuffers(0, SDL_arraysize(buffers), buffers, strides, offsets);

        m_Context->GSSetShader(nullptr, nullptr, 0);

        m_Context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
        glm::mat4 viewProjection = camera.GetViewProjection();
        UpdateBuffer(m_Context.Get(), m_FrameConstants.Get(), viewProjection);
        m_Context->VSSetConstantBuffers(0, 1, m_FrameConstants.GetAddressOf());

        m_Context->PSSetShader(m_PixelShader.Get(), nullptr, 0);
        ID3D11SamplerState* sampler = m_CommonStates->PointClamp();
        m_Context->PSSetSamplers(0, 1, &sampler);

        m_Context->OMSetBlendState(m_CommonStates->NonPremultiplied(), nullptr, 0xFFFFFFFF);
        m_Context->OMSetDepthStencilState(m_CommonStates->DepthNone(), 0);
    }

    {
        ZoneScopedN("Sort");
        std::sort(queue.sprites.begin(), queue.sprites.end(), [](const SpriteDrawRequest& a, const SpriteDrawRequest& b)
        {
            return a.depth < b.depth || a.texture != b.texture;
        });
    }

    auto GetSpriteTexture = [this](const SpriteDrawRequest& sprite)
    {
        return sprite.texture.get() != nullptr
            ? sprite.texture->m_View.Get()
            : m_MissingTexture.m_View.Get();
    };

    for (usize spriteIdx = 0; spriteIdx < queue.sprites.size();)
    {
        ZoneScopedN("Drawcall");

        auto* currentTexture = GetSpriteTexture(queue.sprites[spriteIdx]);
        m_Context->PSSetShaderResources(0, 1, &currentTexture);

        D3D11_MAPPED_SUBRESOURCE spriteInstanceSubres {};
        m_Context->Map(m_SpriteInstanceData.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &spriteInstanceSubres);
        auto* spriteInstanceBuff = (SpriteInstanceData*)spriteInstanceSubres.pData;

        usize spritesInBatch = 0;
        do
        {
            auto& sprite = queue.sprites[spriteIdx];
            auto* texture = GetSpriteTexture(sprite);
            if (texture != currentTexture)
            {
                break;
            }

            auto& spriteInstance = spriteInstanceBuff[spritesInBatch];
            spriteInstance.colorTint = sprite.colorTint;
            spriteInstance.position = sprite.position;
            spriteInstance.uvMin = sprite.uvMin;
            spriteInstance.uvMax = sprite.uvMax;
            spriteInstance.scale = sprite.scale;
            spriteInstance.rotation = glm::radians(sprite.rotation);
            spriteInstance.depth = (float)sprite.depth / 255.0f;

            ++spriteIdx;
            ++spritesInBatch;
        } while (spritesInBatch < MAX_BATCH_SIZE && spriteIdx < queue.sprites.size());

        m_Context->Unmap(m_SpriteInstanceData.Get(), 0);

        ++stats.drawcallCount;
        m_Context->DrawIndexedInstanced(6, spritesInBatch, 0, 0, 0);
    }


    return stats;
}

void RenderContextDX11::Present()
{
    {
        ZoneScoped;
        m_Swapchain->Present(0, 0);
    }

    FrameMark; // notify Tracy Profiler that the frame was rendered
}

void RenderContextDX11::ImGuiBindingsInit()
{
    ImGui_ImplDX11_Init(m_Device.Get(), m_Context.Get());
}

void RenderContextDX11::ImGuiBindingsNewFrame()
{
    ImGui_ImplDX11_NewFrame();
}

void RenderContextDX11::ImGuiBindingsRender(ImDrawData* drawData)
{
    ZoneScoped;
    ImGui_ImplDX11_RenderDrawData(drawData);
}

void RenderContextDX11::ImGuiBindingsShutdown()
{
    ImGui_ImplDX11_Shutdown();
}

void RenderContextDX11::Im3dBindingsInit()
{
    m_Im3dRender = std::make_unique<Im3dDx11>(m_Device, m_Context);
}

void RenderContextDX11::Im3dBindingsNewFrame() {}

void RenderContextDX11::Im3dBindingsRender(const ICamera& camera)
{
    m_Im3dRender->Render(camera, m_Window->GetWidth(), m_Window->GetHeight());
}

void RenderContextDX11::Im3dBindingsShutdown()
{
    m_Im3dRender.reset();
}

RenderContextDX11::RenderContextDX11(std::shared_ptr<Window> window)
    : m_Window(std::move(window))
{
}

TextureHandle RenderContextDX11::LoadTexture(const std::filesystem::path& absolutePath)
{
    auto fileData = LoadFileBytes(absolutePath);
    auto newTexture = std::shared_ptr<TextureData>(new TextureData());
    HRESULT hresult = LoadTextureFromMemory(fileData.data(), fileData.size(), *newTexture);
    CGT_CHECK_HRESULT(hresult, "Couldn't create texture from file at {}", absolutePath);

    return newTexture;
}

HRESULT RenderContextDX11::LoadTextureFromMemory(const u8* data, usize size, TextureData& outData)
{
    HRESULT hresult = DirectX::CreateWICTextureFromMemory(
        m_Device.Get(),
        data,
        size,
        outData.m_Resource.GetAddressOf(),
        outData.m_View.GetAddressOf());

    return hresult;
}

}
