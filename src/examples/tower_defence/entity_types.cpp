#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/entity_types.h>
#include <examples/tower_defence/entities.h>
#include <examples/tower_defence/map_data.h>

void LoadEnemyTypes(tson::Map& map, EnemyTypeCollection& outEnemyTypes)
{
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
}

void LoadTowerTypes(tson::Map& map, TowerTypeCollection& outTowerTypes)
{
    auto* towerLayer = map.getLayer("TowerTypes");
    CGT_ASSERT_ALWAYS(towerLayer && towerLayer->getType() == tson::LayerType::ObjectGroup);

    outTowerTypes.clear();
    for (auto& towerData : towerLayer->getObjectsByType(tson::ObjectType::Object))
    {
        auto& towerType = outTowerTypes.emplace_back();
        towerType.name = towerData.getName();
        towerType.cost = towerData.get<float>("Cost");
        towerType.range = towerData.get<float>("Range");
        towerType.damage = towerData.get<float>("Damage");
        towerType.shotsPerSecond = towerData.get<float>("ShotsPerSecond");
        towerType.splashRadius = towerData.get<float>("SplashRadius");
        towerType.projectileSpeed = towerData.get<float>("ProjectileSpeed");

        towerType.tileId = towerData.getGid();
        towerType.projectileTileId = towerData.get<int>("ProjectileTile");
        towerType.hitTileId = towerData.get<int>("HitTile");
    }
}

void SetupEnemy(const EnemyTypeCollection& enemyTypes, u32 typeIdx, const EnemyPath& path, Enemy& outEnemy)
{
    CGT_ASSERT(typeIdx < enemyTypes.size());
    const EnemyType& type = enemyTypes[typeIdx];

    CGT_ASSERT(path.waypoints.size() > 1);
    glm::vec2 a = path.waypoints[0];
    glm::vec2 b = path.waypoints[1];

    outEnemy.position = a;
    outEnemy.direction = glm::normalize(b - a);
    outEnemy.typeIdx = typeIdx;
    outEnemy.remainingHealth = type.maxHealth;
    outEnemy.targetPointIdx = 1;
}

void SetupTower(const TowerTypeCollection& towerTypes, u32 typeIdx, glm::vec2 position, Tower& outTower)
{
    outTower.position = position;
    outTower.timeSinceLastShot = 0.0f;
    outTower.typeIdx = typeIdx;
}
