#include <engine/render/core/pch.h>

#include <engine/render/core/sprite_draw_list.h>
#include <engine/render/core/i_render_context.h>

namespace cgt::render
{

void SpriteDrawList::SortForRendering(IRenderContext& render)
{
    ZoneScoped;

    std::sort(m_Sprites.begin(), m_Sprites.end(), [&render](const SpriteDrawRequest& a, const SpriteDrawRequest& b)
    {
        usize aKey = render.GetTextureSortKey(a.src.texture);
        usize bKey = render.GetTextureSortKey(b.src.texture);
        return a.layer < b.layer || aKey < bKey;
    });
}

}