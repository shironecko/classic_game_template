#pragma once

#include <engine/render/core/sprite_draw_list.h>

namespace cgt
{

class Engine;

class TilesetHelper final
{
public:
    static std::unique_ptr<TilesetHelper> LoadMapTilesets(tson::Map& map, const std::filesystem::path& baseMapAbsPath, Engine& engine);

    bool GetTileSpriteSrc(u32 tileIdx, cgt::render::SpriteSource& outSrc) const;
    void RenderTileLayers(tson::Map& map, cgt::render::SpriteDrawList& outDrawList, u8 baseSpriteLayer) const;
    void RenderTileLayer(tson::Layer& layer, cgt::render::SpriteDrawList& outDrawList, u8 spriteLayer) const;

private:
    TilesetHelper(tson::Map& map, const std::filesystem::path& baseMapAbsPath, Engine& engine);

    class Tileset
    {
    public:
        static void Load(tson::Map& map, const tson::Tileset& tileset, cgt::render::TextureHandle texture, Tileset& outTileset);

        bool GetTileSpriteSrc(u32 tileIdx, cgt::render::SpriteSource& outSrc) const;

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

        std::vector<float> m_BaseTileRotations;

        cgt::render::TextureHandle m_Texture;
    };

    std::vector<Tileset> m_Tilesets;
};

}
