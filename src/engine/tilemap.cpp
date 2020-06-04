#include <engine/pch.h>

#include <engine/tilemap.h>
#include <engine/assets.h>

namespace cgt
{

std::unique_ptr<Tilemap> Tilemap::LoadFrom(const char* path, render::IRenderContext& render, const char* parentFolder)
{
    auto map = LoadTiledMap(path);
    return std::make_unique<Tilemap>(std::move(map), render, parentFolder);
}

Tilemap::Tilemap(tmx_map* map, render::IRenderContext& render, const char* parentFolder)
    : m_Map(map)
{
    for (auto* tileset = m_Map->ts_head; tileset != nullptr; tileset = tileset->next)
    {
        const char* imagePath = tileset->tileset->image->source;
        auto textureIter = m_Textures.find(imagePath);
        if (textureIter != m_Textures.end())
        {
            continue;
        }

        std::string fullImagePath = fmt::format("{}/{}", parentFolder, imagePath);
        m_Textures[imagePath] = render.LoadTexture(fullImagePath.c_str());
    }
}

Tilemap::~Tilemap()
{
    tmx_map_free(m_Map);
}

void Tilemap::Render(render::RenderQueue& renderQueue)
{
    for (auto* layer = m_Map->ly_head; layer != nullptr; layer = layer->next)
    {
        RenderLayer(renderQueue, *layer);
    }
}

void Tilemap::RenderLayer(render::RenderQueue& queue, const tmx_layer& layer)
{
    if (layer.type != tmx_layer_type::L_LAYER)
    {
        // TODO: add support for other layer types
        return;
    }

    for (u32 y = 0; y < m_Map->height; ++y)
    {
        for (u32 x = 0; x < m_Map->width; ++x)
        {
            usize idx = y * m_Map->width + x;
            auto gid = (layer.content.gids[idx]) & TMX_FLIP_BITS_REMOVAL;
            auto* tile = tmx_get_tile(m_Map, gid);
            if (tile == nullptr)
            {
                continue;
            }

            RenderTile(queue, *tile, x, y);
        }
    }
}

void Tilemap::RenderTile(render::RenderQueue& queue, const tmx_tile& tile, u32 x, u32 y)
{
    auto& tileset = *tile.tileset;

    const u32 minX = tile.ul_x;
    const u32 minY = tile.ul_y;
    const u32 maxX = minX + tileset.tile_width;
    const u32 maxY = minY + tileset.tile_height;
    glm::vec2 uvMin(
        (float)minX / tileset.image->width,
        (float)minY / tileset.image->height);
    glm::vec2 uvMax(
        (float)maxX / tileset.image->width,
        (float)maxY / tileset.image->height);

    auto textureIter = m_Textures.find(tileset.image->source);
    CGT_ASSERT(textureIter != m_Textures.end());

    render::TextureHandle texture;
    if (textureIter != m_Textures.end())
    {
        texture = textureIter->second;
    }

    render::SpriteDrawRequest sprite;
    // TODO: respect actual map drawing order
    sprite.position = glm::vec2(x, y * -1.0f);
    sprite.uvMin = uvMin;
    sprite.uvMax = uvMax;
    sprite.texture = std::move(texture);
    queue.sprites.emplace_back(std::move(sprite));
}

}
