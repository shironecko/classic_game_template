#pragma once

#include <examples/tower_defence/tilemap.h>
#include <examples/tower_defence/map_data.h>

typedef u32 EnemyTypeId;

struct Enemy
{
    glm::vec2 position;
    glm::vec2 direction;

    EnemyTypeId typeId;

    float remainingHealth;

    u32 targetPointIdx;
};

struct EnemyType
{
    float maxHealth;
    float speed;
    float goldReward;
    u32 unitsPerSpawn;
    TileSet::TileId tileId;

    std::string name;

    Enemy CreateEnemy(const EnemyPath& path, EnemyTypeId typeId)
    {
        Enemy enemy;
        const auto a = path.waypoints[0];
        const auto b = path.waypoints[1];
        enemy.position = a;
        enemy.direction = glm::normalize(b - a);
        enemy.typeId = typeId;
        enemy.remainingHealth = maxHealth;
        enemy.targetPointIdx = 1;

        return enemy;
    }
};
