#pragma once

#include <examples/tower_defence/enemy_data.h>
#include <examples/tower_defence/tower_data.h>
#include <examples/tower_defence/map_data.h>

struct PlayerState
{
    float gold;
    u32 lives;
};

struct GameState
{
    PlayerState playerState;
    std::vector<Enemy> enemies;
    std::vector<Tower> towers;

    static void TimeStep(const MapData& mapData, const GameState& initialState, GameState& outNextState, float delta);
    static void Interpolate(const GameState& prevState, const GameState& nextState, GameState& outState, float factor);
};
