#pragma once

namespace cgt::render
{

class Camera
{
public:
    glm::mat4 GetView() const;
    glm::mat4 GetProjection() const;
    glm::mat4 GetViewProjection() const;
    glm::vec3 GetForwardDirection() const;
    glm::vec3 GetUpDirection() const;
    glm::vec3 GetPosition() const;

    glm::vec2 ScreenPointToWorld(u32 screenX, u32 screenY) const;
    glm::vec2 WorldPointToScreen(glm::vec2 world) const;

    float PixelsToWorldUnits(float pixels) const;
    float WorldUnitsToPixels(float worldUnits) const;
    glm::vec2 PixelsToWorldUnits(glm::vec2 pixels) const;
    glm::vec2 WorldUnitsToPixels(glm::vec2 worldUnits) const;

    glm::vec2 position = glm::vec2(0.0f);

    float pixelsPerUnit = 1.0f;
    bool snapToPixel = true;

    glm::uvec2 windowDimensions = glm::uvec2(1);
};

}