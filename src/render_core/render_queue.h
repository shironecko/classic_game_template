#pragma once

namespace cgt::render
{

// backend dependent
class TextureData;

typedef std::shared_ptr<TextureData> TextureHandle;


struct SpriteDrawRequest
{
    glm::vec4 colorTint = glm::vec4(1.0f);
    glm::vec2 position = glm::vec4(0.0f);
    glm::vec2 uvMin = glm::vec4(0.0f);
    glm::vec2 uvMax = glm::vec4(1.0f);
    glm::vec2 scale = glm::vec4(1.0f);
    TextureHandle texture;
    float rotation = 0.0f;
    u8 depth = 0;
};

class RenderQueue : private NonCopyable
{
public:
    void Reset();

    glm::vec4 clearColor;
    std::vector<SpriteDrawRequest> sprites;
};

}
