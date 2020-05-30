#pragma once

namespace cgt::render
{

// backend dependent
typedef usize TextureId;

struct SpriteDrawRequest
{
    glm::vec4 colorTint;
    glm::vec2 position;
    glm::vec2 uv;
    TextureId textureId;
    float rotation;
    u8 depth;
};

class RenderQueue : private NonCopyable
{
public:
    void Reset();

    glm::vec4 clearColor;
    std::vector<SpriteDrawRequest> sprites;
};

}
