#pragma once

#include <examples/tower_defence/tilemap.h>

typedef u32 TowerTypeId;

struct Tower
{
    TowerTypeId type;
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

    TileSet::TileId tileId;
    TileSet::TileId projectileTileId;
    TileSet::TileId hitTileId;

    std::string name;

    Tower CreateTower(glm::vec2 position, TowerTypeId type)
    {
        Tower newTower;
        newTower.position = position;
        newTower.timeSinceLastShot = 0.0f;
        newTower.type = type;

        return newTower;
    }
};
