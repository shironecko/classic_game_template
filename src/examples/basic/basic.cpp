#include <engine/api.h>
#include <render_core/api.h>

class Game : public cgt::IGame
{
public:
    void Initialize(cgt::Engine& engine) override
    {
        m_Camera.windowDimensions = engine.GetWindowDimensions();
        m_Camera.pixelsPerUnit = 16.0f;
    }

    ControlFlow Update(cgt::Engine& engine, float deltaTime, bool quitRequestedByUser) override
    {
        m_Sprites.clear();

        ImGui::ShowDemoWindow();

        auto& testSprite = m_Sprites.AddSprite();
        testSprite.scale = {10.0f, 10.0f};
        engine.RenderSprites(m_Sprites);

        return quitRequestedByUser
            ? cgt::IGame::ControlFlow::TerminateGame
            : cgt::IGame::ControlFlow::ContinueRunning;
    }

    void Shutdown(cgt::Engine& engine) override
    {

    }

    cgt::render::ICamera& GetMainCamera() override { return m_Camera; }

private:
    cgt::render::CameraSimpleOrtho m_Camera;
    cgt::render::SpriteDrawList m_Sprites;
};

int GameMain()
{
    cgt::Engine engine;
    engine.Run<Game>();

    return 0;
}