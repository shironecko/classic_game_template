#pragma once

#include <engine/window.h>
#include <engine/input_helper.h>
#include <engine/event_loop.h>
#include <engine/imgui_helper.h>

#include <render_core/i_render_context.h>

namespace cgt
{

namespace render
{
class IRenderContext;
}

class Window;
class InputHelper;

class Engine;

class IGame
{
public:
    enum class ControlFlow
    {
        ContinueRunning,
        TerminateGame,
    };

    virtual void Initialize(Engine& engine) = 0;
    virtual ControlFlow Update(Engine& engine, float deltaTime, bool quitRequestedByUser) = 0;
    virtual void Shutdown(Engine& engine) = 0;

    virtual render::ICamera& GetMainCamera() = 0;

    virtual ~IGame() = default;
};

class Engine final
{
public:
    Engine(const char* windowTitle = "Classic Game Template", glm::uvec2 windowDimensions = { 1280, 720 });

    template<class TGame>
    void Run()
    {
        m_Game = std::make_unique<TGame>();
        RunInternal();
        m_Game.reset();
    }

    glm::uvec2 GetWindowDimensions() { return m_Window.GetDimensions(); }

    render::TextureHandle LoadTexture(const std::filesystem::path& absolutePath) { return m_Render->LoadTexture(absolutePath); }
    ImTextureID GetImGuiTextureID(const render::TextureHandle& texture) { return m_Render->GetImTextureID(texture); }
    void RenderSprites(render::SpriteDrawList& sprites, bool sortBeforeRendering = true) { m_Render->Submit(sprites, m_Game->GetMainCamera(), m_Window.GetDimensions(), sortBeforeRendering); }

    InputHelper& GetInput() { return m_Input; }

private:
    void RunInternal();

    Window m_Window;
    std::unique_ptr<render::IRenderContext> m_Render;
    ImGuiHelper m_Ui;
    InputHelper m_Input;
    std::unique_ptr<render::ICamera> m_MainCamera;
    std::unique_ptr<IGame> m_Game;
};

}
