#include <engine/pch.h>

#include <engine/engine.h>
#include <engine/clock.h>

namespace cgt
{

Engine::Engine(const char* windowTitle, glm::uvec2 windowDimensions)
    : m_Window(windowTitle, windowDimensions)
    , m_Render(render::IRenderContext::Create(m_Window.GetSDLWindow()))
    , m_Ui(m_Window, *m_Render)
    , m_Input(Input::InputProcessingMode::Consume)
    , m_FrametimeMetric("Frame Time", "s", MetricsGuiMetric::USE_SI_UNIT_PREFIX | MetricsGuiMetric::KNOWN_MIN_VALUE)
{
    m_FrametimePlot.AddMetric(&m_FrametimeMetric);
    m_FrametimePlot.mShowLegendColor = false;
    m_FrametimePlot.mShowLegendAverage = true;
    m_FrametimePlot.mShowLegendMin = false;
    m_FrametimePlot.mMinValue = 0.0f;
    m_FrametimePlot.mBarGraph = true;
    m_FrametimePlot.mVBarGapWidth = 0;
    m_FrametimePlot.mVBarMinWidth = 1;
}

void Engine::RunInternal()
{
    {
        ZoneScopedN("Game::Initialize");
        m_Game->Initialize(*this);
    }

    Clock frameClock;
    auto controlFlow = IGame::ControlFlow::ContinueRunning;
    do
    {
        ZoneScopedN("Frame");

        m_Camera.windowDimensions = m_Window.GetDimensions();
        m_Render->Clear({ 0.0f, 0.0f, 0.0f, 1.0f }, m_Window.GetDimensions());

        const float deltaTime = frameClock.Tick();
        m_Input.NewFrame();

        SDL_Event event;
        bool quitRequested = false;
        while (m_Window.PollEvent(event))
        {
            if (m_Ui.ProcessWindowEvent(event) == WindowEventControlFlow::ConsumeEvent)
            {
                // input is event based so we need to reset it if ImGui suddenly starts to capture events
                // lest we get "sticky keys" bug
                m_Input.Reset();
                continue;
            }

            if (m_Input.ProcessWindowEvent(event) == WindowEventControlFlow::ConsumeEvent)
            {
                continue;
            }

            if (event.type == SDL_QUIT)
            {
                quitRequested = true;
            }
        }

        if (enableDebugShortcuts && m_Input.IsKeyHeld(KeyCode::LeftAlt))
        {
            if (m_Input.IsKeyPressed(KeyCode::P))
            {
                renderPerformanceStats = !renderPerformanceStats;
            }
        }

        m_Ui.NewFrame(m_Window, *m_Render, m_Camera, deltaTime);

        controlFlow = m_Game->Update(*this, deltaTime, quitRequested);

        m_FrametimeMetric.AddNewValue(deltaTime);
        RenderPerformanceStats();

        m_Ui.RenderUi(m_Window, *m_Render, m_Camera);
        m_LastFrameStats = m_Render->Present();
    } while (controlFlow == IGame::ControlFlow::ContinueRunning);

    {
        ZoneScopedN("Game::Shutdown");
        m_Game->Shutdown(*this);
    }
}

void Engine::RenderPerformanceStats()
{
    if (!renderPerformanceStats)
    {
        return;
    }

    ImGui::SetNextWindowSize({ 450, 150 }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize, ImGuiCond_FirstUseEver, { 1, 1 });
    if (ImGui::Begin("Performance", &renderPerformanceStats))
    {
        m_FrametimePlot.UpdateAxes();
        m_FrametimePlot.DrawHistory();

        ImGui::Separator();

        ImGui::Columns(2);
        ImGui::TextUnformatted("Sprites");
        ImGui::NextColumn();
        ImGui::Text("%u", m_LastFrameStats.spriteCount);

        ImGui::NextColumn();
        ImGui::TextUnformatted("Drawcalls");
        ImGui::NextColumn();
        ImGui::Text("%u", m_LastFrameStats.drawcallCount);
        ImGui::Columns();

        ImGui::End();
    }
}

render::TextureHandle Engine::LoadTexture(const std::filesystem::path& absolutePath)
{
    return m_Render->LoadTexture(absolutePath);
}

ImTextureID Engine::GetImGuiTextureID(const render::TextureHandle& texture)
{
    return m_Render->GetImTextureID(texture);
}

void Engine::RenderSprites(render::SpriteDrawList& sprites, bool sortBeforeRendering)
{
    m_Render->Submit(sprites, m_Camera, m_Window.GetDimensions(), sortBeforeRendering);
}

}