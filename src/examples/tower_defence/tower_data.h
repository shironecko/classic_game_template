#pragma once

struct Tower
{
    typedef u32 TypeId;

    TypeId type;
    glm::vec2 position;
    float timeSinceLastShot;
};

struct TowerType
{
    float cost;
    float range;
    float damage;
    float shotsPerSecond;
    float projectileSpeed;
    float splashRadius;

    u32 tileId;
    u32 projectileTileId;
    u32 hitTileId;

    std::string name;
};

typedef std::vector<TowerType> TowerTypeCollection;

void LoadTowerTypes(tson::Map& map, TowerTypeCollection& outTowerTypes);
void SetupTower(const TowerTypeCollection& towerTypes, Tower::TypeId typeId, glm::vec2 position, Tower& outTower);

