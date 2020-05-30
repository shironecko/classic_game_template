#include <render_dx11/pch.h>

#include <render_dx11/render_context_dx11.h>

#include <SDL2/SDL_syswm.h>

namespace cgt::render
{

std::shared_ptr<IRenderContext> IRenderContext::BuildWithConfig(RenderConfig config)
{
    return RenderContextDX11::BuildWithConfig(std::move(config));
}

RenderConfig::RenderConfig(std::shared_ptr<Window> window)
    : m_Window(std::move(window))
{
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
    CGT_ASSERT_ALWAYS(featureLevel == D3D_FEATURE_LEVEL_11_0, "D3D Feature Level 11 is not supported!");

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

    context->m_Context->OMSetRenderTargets(1, context->m_RTView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT viewport {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = (float)context->m_Window->GetWidth();
    viewport.Height = (float)context->m_Window->GetHeight();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    context->m_Context->RSSetViewports(1, &viewport);

    return context;
}

void RenderContextDX11::Submit(RenderQueue& queue)
{
    m_Context->ClearRenderTargetView(m_RTView.Get(), &queue.clearColor.x);
    m_Swapchain->Present(0, 0);
}

RenderContextDX11::~RenderContextDX11()
{

}

RenderContextDX11::RenderContextDX11(std::shared_ptr<Window> window)
    : m_Window(std::move(window))
{
}

}
