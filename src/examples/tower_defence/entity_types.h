#pragma once

class Enemy;
class Tower;
class EnemyPath;

struct EntityType
{
    std::string name;
    u32 tileId = 0;
};

struct EnemyType : EntityType
{
    float maxHealth = 0.0f;
    float speed = 0.0f;
    float goldReward = 0.0f;
    u32 unitsPerSpawn = 0;
};

struct TowerType : EntityType
{
    float cost = 0.0f;
    float range = 0.0f;
    float shotsPerSecond = 0.0f;
    u32 projectileTypeIdx = 0;
};

struct ProjectileType : EntityType
{
    float damage = 0.0f;
    float speed = 0.0f;
    float splashRadius = 0.0f;

    u32 hitTileId = 0;
};

typedef std::vector<EnemyType> EnemyTypeCollection;
typedef std::vector<TowerType> TowerTypeCollection;
typedef std::vector<ProjectileType> ProjectileTypeCollection;

void LoadEnemyTypes(tson::Map& map, EnemyTypeCollection& outEnemyTypes);
void LoadTowerTypes(tson::Map& map, TowerTypeCollection& outTowerTypes);

void LoadEntityTypes(tson::Map& map, EnemyTypeCollection& outEnemyTypes, TowerTypeCollection& outTowerTypes, ProjectileTypeCollection& outProjectileTypes);

void SetupEnemy(const EnemyTypeCollection& enemyTypes, u32 typeIdx, const EnemyPath& path, Enemy& outEnemy);
void SetupTower(const TowerTypeCollection& towerTypes, u32 typeIdx, glm::vec2 position, Tower& outTower);
