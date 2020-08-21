#pragma once

#include <engine/render/core/i_render_context.h>

#include <examples/tower_defence/map_data.h>
#include <examples/tower_defence/game_state.h>

class GameSession
{
public:
    static std::unique_ptr<GameSession> FromMap(const std::filesystem::path mapAbsolutePath, cgt::Engine& engine, float fixedTimeDelta);

    void TimeStep(const GameCommandQueue& commands, GameEventQueue& outGameEvents);
    void InterpolateState(GameState& outState, float amount);

    void RenderWorld(GameState& interpolatedState, cgt::Engine& engine);

    MapData mapData;
    std::unique_ptr<cgt::TilesetHelper> tilesetHelper;

private:
    GameState m_GameStates[2];
    GameState* m_PrevState;
    GameState* m_NextState;

    float m_FixedDelta;

    cgt::render::SpriteDrawList m_StaticMapDrawList;
    cgt::render::SpriteDrawList m_EntitiesDrawList;
};
