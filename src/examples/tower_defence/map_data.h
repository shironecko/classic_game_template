#pragma once

struct EnemyPath
{
    std::string debugName;
    glm::vec4 debugColor;
    std::vector<glm::vec2> waypoints;

    void DebugRender()
    {
        Im3d::PushAlpha(0.5f);
        Im3d::PushColor({ debugColor.r, debugColor.g, debugColor.b, debugColor.a });

        Im3d::Text(glm::vec3(waypoints[0], 0.0f), Im3d::TextFlags_AlignTop, debugName.c_str());

        Im3d::PushSize(3.0f);

        Im3d::BeginLineStrip();
        for (auto point : waypoints)
        {
            Im3d::Vertex(glm::vec3(point, 0.0f));
        }
        Im3d::End();

        Im3d::PopSize();
        Im3d::PopColor();
        Im3d::PopAlpha();
    }
};

class BuildableMap
{
public:
    BuildableMap(const tson::Map& map, const tson::Layer& layer)
        : m_Width(map.getSize().x)
        , m_Height(map.getSize().y)
        , m_Grid(m_Width * m_Height, 0)
    {
        for (auto &[pos, tile] : layer.getTileData())
        {
            bool buildable = tile->get<bool>("BuildingAllowed");

            At(std::get<0>(pos), std::get<1>(pos)) = buildable ? 1 : 0;
        }
    }

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
            (i32)trunc(world.x + 0.5f * glm::sign(world.x)),
            (i32)trunc(world.y + 0.5f * glm::sign(world.y)));

        return tile;
    }

private:
    u32 m_Width;
    u32 m_Height;
    std::vector<u8> m_Grid;
};
