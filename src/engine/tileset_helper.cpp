#include <engine/pch.h>

#include <engine/tileset_helper.h>
#include <engine/engine.h>
#include <engine/assets.h>

namespace cgt
{

void TilesetHelper::Tileset::Load(tson::Map& map, const tson::Tileset& tileset, cgt::render::TextureHandle texture, Tileset& outTileset)
{
    outTileset.m_Texture = std::move(texture);

    outTileset.m_TextureWidth = tileset.getImageSize().x;
    outTileset.m_TextureHeight = tileset.getImageSize().y;

    outTileset.m_Margin = tileset.getMargin();
    outTileset.m_Spacing = tileset.getSpacing();

    outTileset.m_Columns = tileset.getColumns();
    outTileset.m_TileCount = tileset.getTileCount();

    outTileset.m_TileWidth = tileset.getTileSize().x;
    outTileset.m_TileHeight = tileset.getTileSize().y;

    outTileset.m_FirstTileIdx = tileset.getFirstgid();

    outTileset.m_BaseTileRotations.reserve(outTileset.m_TileCount);
    auto& tileMap = map.getTileMap();
    for (u32 i = 0; i < outTileset.m_TileCount; ++i)
    {
        u32 tileId = i + outTileset.m_FirstTileIdx;
        auto* tile = tileMap.at(tileId);
        float baseRotation = tile->get<float>("BaseRotation");
        outTileset.m_BaseTileRotations.emplace_back(baseRotation);
    }
}

bool TilesetHelper::Tileset::GetTileSpriteSrc(u32 tileIdx, render::SpriteSource& outSrc) const
{
    if (tileIdx >= m_FirstTileIdx && tileIdx < m_FirstTileIdx + m_TileCount)
    {
        const u32 idx = tileIdx - m_FirstTileIdx;

        const u32 tileColumn = idx % m_Columns;
        const u32 tileRow = idx / m_Columns;

        const u32 tileX = m_Margin + m_TileWidth * tileColumn + m_Spacing * tileColumn;
        const u32 tileY = m_Margin + m_TileHeight * tileRow + m_Spacing * tileRow;

        const glm::vec2 uvTileDimensions((float)m_TileWidth / m_TextureWidth, (float)m_TileHeight / m_TextureHeight);

        outSrc.texture = m_Texture;
        outSrc.uv.min = glm::vec2((float)tileX / m_TextureWidth, (float)tileY / m_TextureHeight);
        outSrc.uv.max = outSrc.uv.min + uvTileDimensions;

        outSrc.baseRotation = m_BaseTileRotations[idx];

        return true;
    }

    return false;
}

std::unique_ptr<TilesetHelper> TilesetHelper::LoadMapTilesets(tson::Map& map, const std::filesystem::path& baseMapAbsPath, Engine& engine)
{
    auto newTilesetHelper = std::unique_ptr<TilesetHelper>(new TilesetHelper(map, baseMapAbsPath, engine));
    return newTilesetHelper;
}

TilesetHelper::TilesetHelper(tson::Map& map, const std::filesystem::path& baseMapAbsPath, Engine& engine)
{
    for (auto& tileset : map.getTilesets())
    {
        auto texturePath = baseMapAbsPath / tileset.getImagePath();
        auto texture = engine.LoadTexture(texturePath);
        Tileset::Load(map, tileset, std::move(texture), m_Tilesets.emplace_back());
    }
}

bool TilesetHelper::GetTileSpriteSrc(u32 tileIdx, render::SpriteSource& outSrc) const
{
    bool tileFound = false;
    for (const Tileset& ts : m_Tilesets)
    {
        if (ts.GetTileSpriteSrc(tileIdx, outSrc))
        {
            tileFound = true;
            break;
        }
    }

    CGT_ASSERT_MSG(tileFound, "Tile was not found in any of the tilesets, maybe this tile is from a different map?");
    return tileFound;
}

void TilesetHelper::RenderTileLayers(tson::Map& map, cgt::render::SpriteDrawList& outDrawList, u8 baseSpriteLayer) const
{
    u8 spriteLayer = baseSpriteLayer;
    for (auto& mapLayer : map.getLayers())
    {
        if (mapLayer.getType() != tson::LayerType::TileLayer)
        {
            continue;
        }

        RenderTileLayer(mapLayer, outDrawList, spriteLayer);
        ++spriteLayer;
    }
}

void TilesetHelper::RenderTileLayer(tson::Layer& layer, cgt::render::SpriteDrawList& outDrawList, u8 spriteLayer) const
{
    CGT_ASSERT(layer.getType() == tson::LayerType::TileLayer);

    for (auto& [pos, tile] : layer.getTileData())
    {
        float x = float(std::get<0>(pos));

        // TODO: support non-standard render orders?
        float y = float(std::get<1>(pos)) * -1.0f; // reverse Y axis

        auto& sprite = outDrawList.AddSprite();
        GetTileSpriteSrc(tile->getId(), sprite.src);
        sprite.layer = spriteLayer;
        sprite.position = { x, y };
    }
}

}