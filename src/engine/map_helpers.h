#pragma once

#include <render_core/i_render_context.h>

namespace cgt
{

namespace render { class IRenderContext; }

class Tileset
{
public:
    static const u32 INVALID_TILE_IDX = 0xFFFFFFFF;

    static void Load(const tson::Tileset& tileset, cgt::render::TextureHandle texture, Tileset& outTileset);

    bool GetTileSpriteSrc(u32 tileIdx, cgt::render::SpriteSource& outSrc);

private:
    u32 m_TextureWidth;
    u32 m_TextureHeight;

    u32 m_Margin;
    u32 m_Spacing;

    u32 m_Columns;
    u32 m_TileCount;

    u32 m_TileWidth;
    u32 m_TileHeight;

    u32 m_FirstTileIdx;

    cgt::render::TextureHandle m_Texture;
};

class TilesetHelper
{
public:
    static std::unique_ptr<TilesetHelper> LoadMapTilesets(tson::Map& map, const std::filesystem::path& baseMapAbsPath, cgt::render::IRenderContext& render);

    bool GetTileSpriteSrc(u32 tileIdx, cgt::render::SpriteSource& outSrc);
    void RenderTileLayers(tson::Map& map, cgt::render::SpriteDrawList& outDrawList, u8 baseSpriteLayer);

private:
    TilesetHelper(tson::Map& map, const std::filesystem::path& baseMapAbsPath, cgt::render::IRenderContext& render);

    std::vector<Tileset> m_Tilesets;
};

}
