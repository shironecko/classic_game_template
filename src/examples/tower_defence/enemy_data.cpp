#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/enemy_data.h>
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

void SetupEnemy(const EnemyTypeCollection& enemyTypes, Enemy::TypeId typeId, const EnemyPath& path, Enemy& outEnemy)
{
    CGT_ASSERT(typeId < enemyTypes.size());
    const EnemyType& type = enemyTypes[typeId];

    CGT_ASSERT(path.waypoints.size() > 1);
    glm::vec2 a = path.waypoints[0];
    glm::vec2 b = path.waypoints[1];

    outEnemy.position = a;
    outEnemy.direction = glm::normalize(b - a);
    outEnemy.type = typeId;
    outEnemy.remainingHealth = type.maxHealth;
    outEnemy.targetPointIdx = 1;
}
