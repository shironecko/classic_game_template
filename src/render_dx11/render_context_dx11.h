#pragma once

#include <engine/window.h>
#include <render_core/render_config.h>
#include <render_core/i_render_context.h>

namespace cgt::render
{
class RenderContextDX11 : public IRenderContext, private NonCopyable
{
public:
    static std::shared_ptr<RenderContextDX11> BuildWithConfig(RenderConfig config);

    void Submit() override;

    ~RenderContextDX11() override;

private:
    explicit RenderContextDX11(std::shared_ptr<Window> window);

    std::shared_ptr<Window> m_Window;

    ComPtr<ID3D11Device> m_Device;
    ComPtr<ID3D11DeviceContext> m_Context;
    ComPtr<IDXGISwapChain> m_Swapchain;
    ComPtr<ID3D11RenderTargetView> m_RTView;
};
}
