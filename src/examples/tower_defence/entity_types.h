#pragma once

class Enemy;
class Tower;
class EnemyPath;

struct EnemyType
{
    float maxHealth = 0.0f;
    float speed = 0.0f;
    float goldReward = 0.0f;
    uint32_t unitsPerSpawn = 0;
    uint32_t tileId = 0;

    std::string name;
};

struct TowerType
{
    float cost = 0.0f;
    float range = 0.0f;
    float damage = 0.0f;
    float shotsPerSecond = 0.0f;
    float projectileSpeed = 0.0f;
    float splashRadius = 0.0f;

    u32 tileId = 0;
    u32 projectileTileId = 0;
    u32 hitTileId = 0;

    std::string name;
};

typedef std::vector<EnemyType> EnemyTypeCollection;
typedef std::vector<TowerType> TowerTypeCollection;

void LoadEnemyTypes(tson::Map& map, EnemyTypeCollection& outEnemyTypes);
void LoadTowerTypes(tson::Map& map, TowerTypeCollection& outTowerTypes);

void SetupEnemy(const EnemyTypeCollection& enemyTypes, u32 typeIdx, const EnemyPath& path, Enemy& outEnemy);
void SetupTower(const TowerTypeCollection& towerTypes, u32 typeIdx, glm::vec2 position, Tower& outTower);
