#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/tilemap.h>

std::unique_ptr<TileSet> TileSet::Load(const tson::Tileset& tileset, cgt::render::TextureHandle texture)
{
    auto newTileset = std::unique_ptr<TileSet>(new TileSet(tileset, texture));
    return newTileset;
}

SpriteUV TileSet::operator[](TileId id) const
{
    CGT_ASSERT(id < m_TileCount);

    const u32 tileColumn = id % m_Columns;
    const u32 tileRow = id / m_Columns;

    const u32 tileX = m_Margin + m_TileWidth * tileColumn + m_Spacing * tileColumn;
    const u32 tileY = m_Margin + m_TileHeight * tileRow + m_Spacing * tileRow;

    const glm::vec2 uvTileDimensions((float)m_TileWidth / m_TextureWidth, (float)m_TileHeight / m_TextureHeight);

    SpriteUV uv;
    uv.min = glm::vec2((float)tileX / m_TextureWidth, (float)tileY / m_TextureHeight);
    uv.max = uv.min + uvTileDimensions;

    return uv;
}

TileSet::TileSet(const tson::Tileset& tileset, cgt::render::TextureHandle texture)
    : m_Texture(std::move(texture))
{
    m_TextureWidth = tileset.getImageSize().x;
    m_TextureHeight = tileset.getImageSize().y;

    m_Margin = tileset.getMargin();
    m_Spacing = tileset.getSpacing();

    m_Columns = tileset.getColumns();
    m_TileCount = tileset.getTileCount();

    m_TileWidth = tileset.getTileSize().x;
    m_TileHeight = tileset.getTileSize().y;
}

std::unique_ptr<StaticTileGrid> StaticTileGrid::Load(const tson::Map& map, const tson::Layer& layer, const tson::Tileset& tileset, u8 depth)
{
    auto newTileGrid = std::unique_ptr<StaticTileGrid>(new StaticTileGrid(map, layer, tileset, depth));
    return newTileGrid;
}

void StaticTileGrid::Render(cgt::render::RenderQueue& renderQueue, const TileSet& tileset)
{
    ZoneScoped;

    for (usize i = 0, n = m_Tiles.size(); i < n; ++i)
    {
        const u32 x = i % m_Colums;
        const u32 y = i / m_Colums;

        auto tileId = m_Tiles[i];
        if (tileId == TileSet::INVALID_TILE_ID)
        {
            continue;;
        }

        const auto uv = tileset[tileId];

        cgt::render::SpriteDrawRequest sprite;
        sprite.position = glm::vec2(x, y * -1.0f);
        sprite.texture = tileset.GetTexture();
        sprite.uvMin = uv.min;
        sprite.uvMax = uv.max;
        renderQueue.sprites.emplace_back(std::move(sprite));
    }
}

StaticTileGrid::StaticTileGrid(const tson::Map& map, const tson::Layer& layer, const tson::Tileset& tileset, u8 depth)
    : m_Colums(layer.getSize().x)
    , m_Depth(depth)
{
    m_Tiles.resize(m_Colums * layer.getSize().y, TileSet::INVALID_TILE_ID);
    tson::Tileset* currentTileset = nullptr;
    for (auto& [pos, tile] : layer.getTileData())
    {
        u32 tileIdx = std::get<1>(pos) * m_Colums + std::get<0>(pos);
        auto tileGuid = tile->getId();

        CGT_ASSERT_MSG(
            tileGuid >= tileset.getFirstgid() && tileGuid < tileset.getFirstgid() + tileset.getTileCount(),
            "Only one tileset per layer is supported!");

        m_Tiles[tileIdx] = tileGuid - tileset.getFirstgid();
    }
}

