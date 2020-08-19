#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/game_session.h>
#include <examples/tower_defence/helper_functions.h>

std::unique_ptr<GameSession> GameSession::FromMap(const std::filesystem::path mapAbsolutePath, cgt::Engine& engine, float fixedTimeDelta)
{
    auto gameSession = std::unique_ptr<GameSession>(new GameSession());

    tson::Tileson mapParser;
    tson::Map map = mapParser.parse(mapAbsolutePath);
    CGT_ASSERT_ALWAYS(map.getStatus() == tson::ParseStatus::OK);

    auto mapBasePath = mapAbsolutePath;
    mapBasePath.remove_filename();
    gameSession->tilesetHelper = cgt::TilesetHelper::LoadMapTilesets(map, mapBasePath, engine);

    cgt::render::SpriteDrawList staticMapDrawList;
    gameSession->tilesetHelper->RenderTileLayers(map, gameSession->m_StaticMapDrawList, 0);

    MapData::Load(map, gameSession->mapData);

    i32 startingGold = map.get<i32>("StartingGold");
    i32 startingLives = map.get<i32>("StartingLives");

    gameSession->m_PrevState = &gameSession->m_GameStates[0];
    gameSession->m_NextState = &gameSession->m_GameStates[1];

    gameSession->m_NextState->playerState.gold = startingGold;
    gameSession->m_NextState->playerState.lives = startingLives;
    gameSession->m_FixedDelta = fixedTimeDelta;

    // warm up the game state by doing one timestep immediately
    GameCommandQueue dummyCommandQueue;
    GameEventQueue dummyEventQueue;
    gameSession->TimeStep(dummyCommandQueue, dummyEventQueue);

    return gameSession;
}

void GameSession::TimeStep(const GameCommandQueue& commands, GameEventQueue& outGameEvents)
{
    std::swap(m_PrevState, m_NextState);
    GameState::TimeStep(mapData, *m_PrevState, *m_NextState, commands, outGameEvents, m_FixedDelta);
}

void GameSession::InterpolateState(GameState& outState, float amount)
{
    GameState::Interpolate(*m_PrevState, *m_NextState, outState, amount);
}

void GameSession::RenderWorld(GameState& interpolatedState, cgt::Engine& engine)
{
    m_EntitiesDrawList.clear();

    interpolatedState.ForEachEntity(mapData, [&](auto& entity, auto& type) {
        RenderEntity(entity, type, *tilesetHelper, m_EntitiesDrawList);
    });

    engine.RenderSprites(m_StaticMapDrawList, false);
    engine.RenderSprites(m_EntitiesDrawList, false);
}
