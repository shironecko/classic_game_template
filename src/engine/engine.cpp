#include <engine/pch.h>

#include <engine/engine.h>
#include <engine/clock.h>
#include <render_core/i_render_context.h>
#include <render_core/camera_simple_ortho.h>

namespace cgt
{

Engine::Engine(const char* windowTitle, glm::uvec2 windowDimensions)
    : m_Window(windowTitle, windowDimensions)
    , m_Render(render::IRenderContext::Create(m_Window.GetSDLWindow()))
    , m_Ui(m_Window, *m_Render)
    , m_Input(InputHelper::InputProcessingMode::Consume)
{
}

void Engine::RunInternal()
{
    m_Game->Initialize(*this);

    Clock frameClock;
    auto controlFlow = IGame::ControlFlow::ContinueRunning;
    do
    {
        ZoneScopedN("Frame");

        m_Render->Clear({ 0.0f, 0.0f, 0.0f, 1.0f }, m_Window.GetDimensions());

        const float deltaTime = frameClock.Tick();
        m_Input.NewFrame();

        SDL_Event event;
        bool quitRequested = false;
        while (m_Window.PollEvent(event))
        {
            if (m_Ui.OnEvent(event) == IEventListener::EventAction::Consume)
            {
                // input is event based so we need to reset it if ImGui suddenly starts to capture events
                // lest we get "sticky keys" bug
                m_Input.Reset();
                continue;
            }

            if (m_Input.OnEvent(event) == IEventListener::EventAction::Consume)
            {
                continue;
            }

            if (event.type == SDL_QUIT)
            {
                quitRequested = true;
            }
        }

        auto& mainCamera = m_Game->GetMainCamera();
        m_Ui.NewFrame(m_Window, *m_Render, mainCamera, deltaTime);

        controlFlow = m_Game->Update(*this, deltaTime, quitRequested);

        m_Ui.RenderUi(*m_Render, mainCamera, m_Window.GetDimensions());
        m_Render->Present();
    } while (controlFlow == IGame::ControlFlow::ContinueRunning);

    m_Game->Shutdown(*this);
}

}