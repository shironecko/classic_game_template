#pragma once

#include <engine/window.h>
#include <engine/input.h>
#include <engine/ui_bindings.h>
#include <engine/render/core/sprite_draw_list.h>
#include <engine/render/core/camera.h>

namespace cgt
{

namespace render
{
class IRenderContext;
}

class Window;
class Input;

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

    render::TextureHandle LoadTexture(const std::filesystem::path& absolutePath);
    ImTextureID GetImGuiTextureID(const render::TextureHandle& texture);
    void RenderSprites(render::SpriteDrawList& sprites, bool sortBeforeRendering = true);

    Input& GetInput() { return m_Input; }
    render::Camera& GetCamera() { return m_Camera; }

private:
    void RunInternal();

    Window m_Window;
    std::unique_ptr<render::IRenderContext> m_Render;
    UIBindings m_Ui;
    Input m_Input;
    render::Camera m_Camera;
    std::unique_ptr<IGame> m_Game;
};

}
