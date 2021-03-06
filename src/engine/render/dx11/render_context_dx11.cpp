#include <engine/render/dx11/pch.h>

#include <engine/render/dx11/render_context_dx11.h>
#include <engine/render/dx11/im3d_dx11.h>
#include <engine/render/dx11/util.h>
#include <engine/assets.h>

#include <SDL2/SDL_syswm.h>

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
};

static std::unique_ptr<IRenderContext> Create();
std::unique_ptr<IRenderContext> IRenderContext::Create(SDL_Window* window)
{
    return RenderContextDX11::Create(window);
}

std::unique_ptr<RenderContextDX11> RenderContextDX11::Create(SDL_Window* window)
{
    ZoneScoped;

    auto context = std::unique_ptr<RenderContextDX11>(new RenderContextDX11());

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
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;

    int windowWidth, windowHeight;
    SDL_GL_GetDrawableSize(window, &windowWidth, &windowHeight);
    DXGI_MODE_DESC modeDesc {};
    modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    modeDesc.Width = windowWidth;
    modeDesc.Height = windowHeight;
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

    D3D11_BLEND_DESC blendStateDesc {};
    blendStateDesc.RenderTarget[0].BlendEnable = true;
    blendStateDesc.RenderTarget[0].SrcBlend  = blendStateDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlend = blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp   = blendStateDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hresult = context->m_Device->CreateBlendState(&blendStateDesc, context->m_BlendState.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create a blend state!");

    D3D11_SAMPLER_DESC samplerDesc {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
    samplerDesc.MaxLOD = FLT_MAX;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    hresult = context->m_Device->CreateSamplerState(&samplerDesc, context->m_Sampler.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create a sampler state!");

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc {};
    depthStencilDesc.DepthEnable = FALSE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace = depthStencilDesc.FrontFace;
    hresult = context->m_Device->CreateDepthStencilState(&depthStencilDesc, context->m_DepthStencil.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create a depth-stencil state!");

    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.MultisampleEnable = TRUE;
    hresult = context->m_Device->CreateRasterizerState(&rasterizerDesc, context->m_Rasterizer.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create a rasterizer state!");

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
        };

    hresult = context->m_Device->CreateInputLayout(
        inputElementDesc,
        CGT_ARRAY_LENGTH(inputElementDesc),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        context->m_InputLayout.GetAddressOf());
    CGT_CHECK_HRESULT(hresult, "Failed to create shader input layout!");

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

    context->m_SpriteInstanceData = CreateBuffer(
        context->m_Device.Get(),
        nullptr,
        sizeof(SpriteInstanceData) * MAX_BATCH_SIZE,
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_WRITE);

    context->m_FrameConstants = CreateBuffer(
        context->m_Device.Get(),
        nullptr,
        sizeof(glm::mat4),
        D3D11_BIND_CONSTANT_BUFFER,
        D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_WRITE);

    return context;
}

void RenderContextDX11::Clear(glm::vec4 clearColor, glm::uvec2 windowDimensions)
{
    ZoneScoped;

    SetUpRenderTarget(windowDimensions);
    m_Context->ClearRenderTargetView(m_RTView.Get(), &clearColor.x);
}

void RenderContextDX11::Submit(SpriteDrawList& drawList, const Camera& camera, glm::uvec2 windowDimensions, bool sortBeforeRendering)
{
    ZoneScoped;

    {
        ZoneScopedN("Setup");
        m_Stats.spriteCount += drawList.size();

        SetUpRenderTarget(windowDimensions);

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
        m_Context->IASetVertexBuffers(0, CGT_ARRAY_LENGTH(buffers), buffers, strides, offsets);

        m_Context->GSSetShader(nullptr, nullptr, 0);

        m_Context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
        glm::mat4 viewProjection = camera.GetViewProjection();
        UpdateBuffer(m_Context.Get(), m_FrameConstants.Get(), viewProjection);
        m_Context->VSSetConstantBuffers(0, 1, m_FrameConstants.GetAddressOf());

        m_Context->PSSetShader(m_PixelShader.Get(), nullptr, 0);
        m_Context->PSSetSamplers(0, 1, m_Sampler.GetAddressOf());

        m_Context->OMSetBlendState(m_BlendState.Get(), nullptr, 0xFFFFFFFF);
        m_Context->OMSetDepthStencilState(m_DepthStencil.Get(), 0);
    }

    if (sortBeforeRendering)
    {
        drawList.SortForRendering(*this);
    }

    auto GetSpriteTexture = [this](const SpriteDrawRequest& sprite)
    {
        return sprite.src.texture.get() != nullptr
            ? sprite.src.texture->m_View.Get()
            : m_MissingTexture.m_View.Get();
    };

    for (usize spriteIdx = 0; spriteIdx < drawList.size();)
    {
        ZoneScopedN("Drawcall");

        auto* currentTexture = GetSpriteTexture(drawList[spriteIdx]);
        m_Context->PSSetShaderResources(0, 1, &currentTexture);

        D3D11_MAPPED_SUBRESOURCE spriteInstanceSubres {};
        m_Context->Map(m_SpriteInstanceData.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &spriteInstanceSubres);
        auto* spriteInstanceBuff = (SpriteInstanceData*)spriteInstanceSubres.pData;

        usize spritesInBatch = 0;
        do
        {
            auto& sprite = drawList[spriteIdx];
            auto* texture = GetSpriteTexture(sprite);
            if (texture != currentTexture)
            {
                break;
            }

            auto& spriteInstance = spriteInstanceBuff[spritesInBatch];
            spriteInstance.colorTint = sprite.colorTint;
            spriteInstance.position = sprite.position;
            spriteInstance.uvMin = sprite.src.uv.min;
            spriteInstance.uvMax = sprite.src.uv.max;
            spriteInstance.scale = sprite.scale;
            spriteInstance.rotation = glm::radians(sprite.rotation - sprite.src.baseRotation);

            ++spriteIdx;
            ++spritesInBatch;
        } while (spritesInBatch < MAX_BATCH_SIZE && spriteIdx < drawList.size());

        m_Context->Unmap(m_SpriteInstanceData.Get(), 0);

        ++m_Stats.drawcallCount;
        m_Context->DrawIndexedInstanced(6, spritesInBatch, 0, 0, 0);
    }
}

void RenderContextDX11::SetUpRenderTarget(glm::uvec2 windowDimensions)
{
    D3D11_VIEWPORT viewport {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = (float)windowDimensions.x;
    viewport.Height = (float)windowDimensions.y;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_Context->RSSetViewports(1, &viewport);
    m_Context->RSSetState(m_Rasterizer.Get());

    m_Context->OMSetRenderTargets(1, m_RTView.GetAddressOf(), nullptr);
}

RenderStats RenderContextDX11::Present()
{
    {
        ZoneScoped;
        m_Swapchain->Present(0, 0);
    }

    RenderStats stats = m_Stats;
    m_Stats.Reset();

    FrameMark; // notify Tracy Profiler that the frame was rendered
    return stats;
}

void RenderContextDX11::ImGuiBindingsInit(SDL_Window* window)
{
    ZoneScoped;

    ImGui_ImplSDL2_InitForD3D(window);
    ImGui_ImplDX11_Init(m_Device.Get(), m_Context.Get());
}

void RenderContextDX11::ImGuiBindingsNewFrame()
{
    ZoneScoped;

    ImGui_ImplDX11_NewFrame();
}

void RenderContextDX11::ImGuiBindingsRender(ImDrawData* drawData)
{
    ZoneScoped;

    ImGui_ImplDX11_RenderDrawData(drawData);
}

void RenderContextDX11::ImGuiBindingsShutdown()
{
    ZoneScoped;

    ImGui_ImplDX11_Shutdown();
}

void RenderContextDX11::Im3dBindingsInit()
{
    m_Im3dRender = std::make_unique<Im3dDx11>(m_Device, m_Context);
}

void RenderContextDX11::Im3dBindingsNewFrame() {}

void RenderContextDX11::Im3dBindingsRender(const Camera& camera, glm::uvec2 windowDimensions)
{
    m_Im3dRender->Render(camera, windowDimensions.x, windowDimensions.y);
}

void RenderContextDX11::Im3dBindingsShutdown()
{
    m_Im3dRender.reset();
}

TextureHandle RenderContextDX11::LoadTexture(const std::filesystem::path& absolutePath)
{
    ZoneScoped;

    auto fileData = LoadFileBytes(absolutePath);
    auto newTexture = std::shared_ptr<TextureData>(new TextureData());
    HRESULT hresult = LoadTextureFromMemory(fileData.data(), fileData.size(), *newTexture);
    CGT_CHECK_HRESULT(hresult, "Couldn't create texture from file at {}", absolutePath);

    return newTexture;
}

HRESULT RenderContextDX11::LoadTextureFromMemory(const u8* data, usize size, TextureData& outData)
{
    SDL_Surface* rawImage = IMG_Load_RW(SDL_RWFromConstMem(data, size), 1);
    if (rawImage->format->format != SDL_PIXELFORMAT_RGBA32)
    {
        ZoneScopedN("Convert Texture");

        SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(rawImage, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(rawImage);
        rawImage = convertedSurface;
    }

    D3D11_TEXTURE2D_DESC desc {};
    desc.Width = rawImage->w;
    desc.Height = rawImage->h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData {};
    initData.pSysMem = rawImage->pixels;
    initData.SysMemPitch = rawImage->pitch;

    ComPtr<ID3D11Texture2D> textureResource;
    HRESULT hresult = m_Device->CreateTexture2D(&desc, &initData, textureResource.GetAddressOf());
    SDL_FreeSurface(rawImage);

    if (!SUCCEEDED(hresult))
    {
        return hresult;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hresult = m_Device->CreateShaderResourceView(textureResource.Get(), &srvDesc, outData.m_View.GetAddressOf());

    return hresult;
}

ImTextureID RenderContextDX11::GetImTextureID(const TextureHandle& texture)
{
    return texture->m_View.Get();
}

usize RenderContextDX11::GetTextureSortKey(const TextureHandle& texture)
{
    usize uPtr = reinterpret_cast<usize>(texture.get() ? texture->m_View.Get() : 0);
    return uPtr;
}

}
