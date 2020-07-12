#pragma once

#include <examples/tower_defence/entities.h>
#include <examples/tower_defence/map_data.h>

struct GameCommand
{
    enum class Type
    {
        Debug_SpawnEnemy,
        Debug_DespawnAllEnemies,
        Debug_AddGold,

        BuildTower,
    } type;

    union CommandData
    {
        struct Debug_SpawnEnemyData
        {
            Enemy::TypeId enemyType;
        } debug_spawnEnemyData;

        struct Debug_AddGoldData
        {
            float amount;
        } debug_addGoldData;

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
    std::vector<Projectile> projectiles;

    u32 nextObjectId = 0;

    static void TimeStep(const MapData& mapData, const GameState& initialState, GameState& outNextState, const GameCommandQueue& commands, float delta);
    static void Interpolate(const GameState& prevState, const GameState& nextState, GameState& outState, float factor);

    static void QueryEnemiesInRadius(const std::vector<Enemy>& enemies, glm::vec2 position, float radius, std::vector<u32>& outResults);
};
