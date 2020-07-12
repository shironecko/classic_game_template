#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/tower_data.h>

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

void SetupTower(const TowerTypeCollection& towerTypes, Tower::TypeId typeId, glm::vec2 position, Tower& outTower)
{
    outTower.position = position;
    outTower.timeSinceLastShot = 0.0f;
    outTower.typeIdx = typeId;
}
