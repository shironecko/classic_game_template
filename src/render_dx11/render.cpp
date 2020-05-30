#include <render_dx11/pch.h>

#include <render_dx11/render.h>

namespace cgt::render
{

std::shared_ptr<IRenderContext> IRenderContext::BuildWithConfig(RenderConfig config)
{
    return RenderContext::BuildWithConfig(std::move(config));
}

RenderConfig::RenderConfig(std::shared_ptr<Window> window)
    : m_Window(std::move(window))
{
}

std::shared_ptr<RenderContext> RenderContext::BuildWithConfig(RenderConfig config)
{
    auto context = std::shared_ptr<RenderContext>(new RenderContext(config.GetSDLWindow()));

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

    return context;
}

RenderContext::~RenderContext()
{

}

RenderContext::RenderContext(std::shared_ptr<Window> window)
    : m_Window(std::move(window))
{
}

}
