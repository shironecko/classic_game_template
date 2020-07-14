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
            u32 enemyType;
        } debug_spawnEnemyData;

        struct Debug_AddGoldData
        {
            float amount;
        } debug_addGoldData;

        struct BuildTowerData
        {
            u32 towerType;
            glm::vec2 position;
        } buildTowerData;
    } data;
};

typedef std::vector<GameCommand> GameCommandQueue;

struct GameEvent
{
    enum class Type
    {
        TowerBuilt,
        ProjectileLaunched,
        ProjectileHit,
        EnemyDied,
    } type;

    union EventData
    {
        struct TowerBuiltData
        {
            glm::vec2 position;
            u32 typeIdx;
        } towerBuiltData;

        struct ProjectileLaunchedData
        {
            glm::vec2 position;
            u32 typeIdx;
        } projectileLaunchedData;

        struct ProjectileHitData
        {
            glm::vec2 position;
            u32 enemyIndex;
            u32 projectileTypeIdx;
        } projectileHitData;

        struct EnemyDiedData
        {
            u32 enemyIndex;
        } enemyDiedData;
    } data;
};

typedef std::vector<GameEvent> GameEventQueue;

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

    static void TimeStep(const MapData& mapData, const GameState& initialState, GameState& outNextState, const GameCommandQueue& commands, GameEventQueue& outGameEvents, float delta);
    static void Interpolate(const GameState& prevState, const GameState& nextState, GameState& outState, float amount);

    static void QueryEnemiesInRadius(const std::vector<Enemy>& enemies, glm::vec2 position, float radius, std::vector<u32>& outResults);

    void ForEachEntity(const MapData& mapData, std::function<void(const Entity&, const EntityType&)> function) const;
    void ForEachEnemy(const MapData& mapData, std::function<void(const Enemy&, const EnemyType&)> function) const;
};
