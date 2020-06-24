#pragma once

struct SpriteUV
{
    glm::vec2 min;
    glm::vec2 max;
};

class TileSet
{
public:
    typedef u32 TileId;

    static const u32 INVALID_TILE_ID = 0xFFFFFFFF;

    static std::unique_ptr<TileSet> Load(const tson::Tileset& tileset, cgt::render::TextureHandle texture);

    SpriteUV operator[](TileId id) const;

    cgt::render::TextureHandle GetTexture() const { return m_Texture; }

private:
    TileSet(const tson::Tileset& tileset, cgt::render::TextureHandle texture);

    u32 m_TextureWidth;
    u32 m_TextureHeight;

    u32 m_Margin;
    u32 m_Spacing;

    u32 m_Columns;
    u32 m_TileCount;

    u32 m_TileWidth;
    u32 m_TileHeight;

    cgt::render::TextureHandle m_Texture;
};

class StaticTileGrid
{
public:
    static std::unique_ptr<StaticTileGrid> Load(const tson::Map& map, const tson::Layer& layer, const tson::Tileset& tileset, u8 depth);

    void Render(cgt::render::RenderQueue& renderQueue, const TileSet& tileset);

private:
    StaticTileGrid(const tson::Map& map, const tson::Layer& layer, const tson::Tileset& tileset, u8 depth);

    u32 m_Colums;
    std::vector<TileSet::TileId> m_Tiles;

    u8 m_Depth;
};