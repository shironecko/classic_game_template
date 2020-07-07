#include <render_core/pch.h>

#include <render_core/sprite_draw_list.h>
#include <render_core/i_render_context.h>

namespace cgt::render
{

void SpriteDrawList::SortForRendering(IRenderContext& render)
{
    ZoneScoped;

    std::sort(m_Sprites.begin(), m_Sprites.end(), [&render](const SpriteDrawRequest& a, const SpriteDrawRequest& b)
    {
        usize aKey = render.GetTextureSortKey(a.texture);
        usize bKey = render.GetTextureSortKey(b.texture);
        return a.layer < b.layer || aKey < bKey;
    });
}

}