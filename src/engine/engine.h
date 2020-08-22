#pragma once

#include <engine/window.h>
#include <engine/input.h>
#include <engine/audio.h>
#include <engine/ui_bindings.h>
#include <engine/ui_extensions.h>
#include <engine/render/core/sprite_draw_list.h>
#include <engine/render/core/camera.h>
#include <engine/render/core/i_render_context.h>

namespace cgt
{

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

template<class TGame>
void RunGame(const char* windowTitle = "Classic Game Template", glm::uvec2 windowDimensions = { 1280, 720 });

class Engine final
{
public:
    glm::uvec2 GetWindowDimensions() { return m_Window.GetDimensions(); }

    render::TextureHandle LoadTexture(const std::filesystem::path& absolutePath);
    ImTextureID GetImGuiTextureID(const render::TextureHandle& texture);
    void RenderSprites(render::SpriteDrawList& sprites, bool sortBeforeRendering = true);

    Input& GetInput() { return m_Input; }
    Audio& GetAudio() { return m_Audio; }
    render::Camera& GetCamera() { return m_Camera; }

    bool enableDebugShortcuts = true;
    bool renderPerformanceStats = true;

private:
    template<class TGame>
    friend void RunGame(const char*, glm::uvec2);

    Engine(const char* windowTitle, glm::uvec2 windowDimensions);

    void RunInternal();

    void RenderPerformanceStats();

    Window m_Window;
    std::unique_ptr<render::IRenderContext> m_Render;
    UIBindings m_Ui;
    Input m_Input;
    render::Camera m_Camera;
    Audio m_Audio;
    std::unique_ptr<IGame> m_Game;

    MetricsGuiMetric m_FrametimeMetric;
    MetricsGuiPlot m_FrametimePlot;

    render::RenderStats m_LastFrameStats;
};

template<class TGame>
void RunGame(const char* windowTitle, glm::uvec2 windowDimensions)
{
    std::unique_ptr<Engine> engine;
    {
        ZoneScopedN("Engine Construction");
        engine = std::unique_ptr<Engine>(new Engine(windowTitle, windowDimensions));
    }

    {
        ZoneScopedN("Game Construction");
        engine->m_Game = std::make_unique<TGame>();
    }

    engine->RunInternal();

    {
        ZoneScopedN("Game Destruction");
        engine->m_Game.reset();
    }

    {
        ZoneScopedN("Engine Destruction");
        engine.reset();
    }
}

}
