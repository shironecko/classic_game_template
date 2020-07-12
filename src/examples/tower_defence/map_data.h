#pragma once

#include <examples/tower_defence/entity_types.h>

struct EnemyPath
{
    std::string debugName;
    glm::vec4 debugColor;
    std::vector<glm::vec2> waypoints;

    static void Load(tson::Map& map, EnemyPath& outPath);

    void DebugRender();
};

class BuildableMap
{
public:
    static void Load(tson::Map& map, BuildableMap& outMap);

    u8& At(u32 x, u32 y)
    {
        CGT_ASSERT(x < m_Width && y < m_Height);
        return m_Grid[y * m_Width + x];
    }

    bool Query(glm::vec2 position)
    {
        auto tile = WorldToTile(position);
        tile.y *= -1;
        if (tile.x < 0 || tile.x >= m_Width || tile.y < 0 || tile.y >= m_Height)
        {
            return false;
        }

        return At((u32)tile.x, (u32)tile.y) == 1;
    }

    glm::ivec2 WorldToTile(glm::vec2 world)
    {
        const glm::ivec2 tile(
            (i32)glm::trunc(world.x + 0.5f * glm::sign(world.x)),
            (i32)glm::trunc(world.y + 0.5f * glm::sign(world.y)));

        return tile;
    }

private:
    u32 m_Width;
    u32 m_Height;
    std::vector<u8> m_Grid;
};

struct MapData
{
    EnemyPath enemyPath;
    EnemyTypeCollection enemyTypes;
    TowerTypeCollection towerTypes;
    ProjectileTypeCollection projectileTypes;
    BuildableMap buildableMap;

    static void Load(tson::Map& map, MapData& outMapData);
};
