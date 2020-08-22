#pragma once

#include <examples/tower_defence/entity_types.h>
#include <examples/tower_defence/entities.h>
#include <examples/tower_defence/map_data.h>
#include <examples/tower_defence/game_state.h>
#include <examples/tower_defence/game_session.h>

class Game : public cgt::IGame
{
public:
    void Initialize(cgt::Engine& engine) override;
    ControlFlow Update(cgt::Engine& engine, float deltaTime, bool quitRequestedByUser) override;
    void Shutdown(cgt::Engine& engine) override;

private:
    // https://www.gafferongames.com/post/fix_your_timestep
    const float FIXED_DELTA = 1.0f / 30.0f;

    GameCommandQueue m_GameCommands;
    GameEventQueue m_GameEvents;
    GameState m_InterpolatedState;

    std::unique_ptr<GameSession> m_GameSession;
    cgt::render::SpriteDrawList m_EffectsDrawList;

    std::unique_ptr<cgt::Music> m_Music;
    std::unique_ptr<cgt::SoundEffect> m_EnemyDeathSound;
};


