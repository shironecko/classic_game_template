#pragma once

struct Tower
{
    typedef u32 TypeId;

    TypeId type = 0;
    glm::vec2 position = glm::vec2(0.0f);
    float rotation = 0.0f;
    float timeSinceLastShot = 0.0f;
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

typedef std::vector<TowerType> TowerTypeCollection;

void LoadTowerTypes(tson::Map& map, TowerTypeCollection& outTowerTypes);
void SetupTower(const TowerTypeCollection& towerTypes, Tower::TypeId typeId, glm::vec2 position, Tower& outTower);

