#pragma once

#include <examples/tower_defence/enemy_data.h>
#include <examples/tower_defence/tower_data.h>
#include <examples/tower_defence/map_data.h>

struct GameCommand
{
    enum class Type
    {
        Debug_SpawnEnemy,
        Debug_DespawnAllEnemies,

        BuildTower,
    } type;

    union CommandData
    {
        struct Debug_SpawnEnemyData
        {
            Enemy::TypeId enemyType;
        } debug_spawnEnemyData;

        struct BuildTowerData
        {
            Tower::TypeId towerType;
            glm::vec2 position;
        } buildTowerData;
    } data;
};

typedef std::vector<GameCommand> GameCommandQueue;

struct PlayerState
{
    float gold;
    u32 lives;
};

struct GameState
{
    std::default_random_engine randomEngine;

    PlayerState playerState;
    std::vector<Enemy> enemies;
    std::vector<Tower> towers;

    static void TimeStep(const MapData& mapData, const GameState& initialState, GameState& outNextState, const GameCommandQueue& commands, float delta);
    static void Interpolate(const GameState& prevState, const GameState& nextState, GameState& outState, float factor);
};
