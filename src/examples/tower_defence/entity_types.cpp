#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/entity_types.h>
#include <examples/tower_defence/entities.h>
#include <examples/tower_defence/map_data.h>

void SetupEnemy(const EnemyTypeCollection& enemyTypes, u32 typeIdx, const EnemyPath& path, Enemy& outEnemy)
{
    CGT_ASSERT(typeIdx < enemyTypes.size());
    const EnemyType& type = enemyTypes[typeIdx];

    CGT_ASSERT(path.waypoints.size() > 1);
    glm::vec2 a = path.waypoints[0];
    glm::vec2 b = path.waypoints[1];

    outEnemy.position = a;
    outEnemy.typeIdx = typeIdx;
    outEnemy.remainingHealth = type.maxHealth;
}

void SetupTower(const TowerTypeCollection& towerTypes, u32 typeIdx, glm::vec2 position, Tower& outTower)
{
    outTower.position = position;
    outTower.timeSinceLastShot = 0.0f;
    outTower.typeIdx = typeIdx;
}

void LoadEntityTypes(tson::Map& map, EnemyTypeCollection& outEnemyTypes, TowerTypeCollection& outTowerTypes,
    ProjectileTypeCollection& outProjectileTypes)
{
    // enemies
    auto* enemyLayer = map.getLayer("EnemyTypes");
    CGT_ASSERT_ALWAYS(enemyLayer && enemyLayer->getType() == tson::LayerType::ObjectGroup);

    outEnemyTypes.clear();
    for (auto& enemyData : enemyLayer->getObjectsByType(tson::ObjectType::Object))
    {
        auto& enemyType = outEnemyTypes.emplace_back();
        enemyType.name = enemyData.getName();
        enemyType.tileId = enemyData.getGid();
        enemyType.maxHealth = enemyData.get<float>("Health");
        enemyType.speed = enemyData.get<float>("Speed");
        enemyType.goldReward = enemyData.get<float>("GoldReward");
        enemyType.unitsPerSpawn = enemyData.get<int>("UnitsPerSpawn");
    }

    // towers and projectiles
    auto* towerLayer = map.getLayer("TowerTypes");
    CGT_ASSERT_ALWAYS(towerLayer && towerLayer->getType() == tson::LayerType::ObjectGroup);

    outTowerTypes.clear();
    outProjectileTypes.clear();
    for (auto& towerData : towerLayer->getObjectsByType(tson::ObjectType::Object))
    {
        auto& towerType = outTowerTypes.emplace_back();
        towerType.name = towerData.getName();
        towerType.tileId = towerData.getGid();
        towerType.cost = towerData.get<float>("Cost");
        towerType.range = towerData.get<float>("Range");
        towerType.shotsPerSecond = towerData.get<float>("ShotsPerSecond");

        auto& projectileType = outProjectileTypes.emplace_back();
        towerType.projectileTypeIdx = outProjectileTypes.size() - 1;

        projectileType.name = fmt::format("{}_Projectile", towerType.name);
        projectileType.tileId = towerData.get<int>("ProjectileTile");
        projectileType.damage = towerData.get<float>("Damage");
        projectileType.splashRadius = towerData.get<float>("SplashRadius");
        projectileType.speed = towerData.get<float>("ProjectileSpeed");
        projectileType.hitTileId = towerData.get<int>("HitTile");
    }
}
