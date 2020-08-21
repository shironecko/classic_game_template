#pragma once

#include <engine/math.h>

namespace cgt::render
{

class IRenderContext;

// backend dependent
class TextureData;

typedef std::shared_ptr<TextureData> TextureHandle;

struct SpriteSource
{
    TextureHandle texture;
    cgt::math::AABB uv = { glm::vec2(0.0f), glm::vec2(1.0f) };
    float baseRotation = 0.0f;
};

struct SpriteDrawRequest
{
    SpriteSource src;
    glm::vec4 colorTint = glm::vec4(1.0f);
    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 scale = glm::vec2(1.0f);
    float rotation = 0.0f;
    u8 layer = 0;
};

class SpriteDrawList : private NonCopyable
{
public:
    typedef std::vector<SpriteDrawRequest> SpriteList;

    SpriteDrawRequest& AddSprite() { return m_Sprites.emplace_back(); }
    void SortForRendering(IRenderContext& render);

    SpriteList::const_iterator begin() const { return m_Sprites.begin(); }
    SpriteList::const_iterator end() const { return m_Sprites.end(); }
    SpriteList::size_type size() const { return m_Sprites.size(); }
    const SpriteDrawRequest& operator[](SpriteList::size_type idx) const { return m_Sprites[idx]; }

    void clear() { m_Sprites.clear(); }

private:
    std::vector<SpriteDrawRequest> m_Sprites;
};

}
