#include <engine/render/core/pch.h>

#include <engine/render/core/camera.h>
#include <engine/window.h>

namespace cgt::render
{

glm::mat4 Camera::GetView() const
{
    const float unitsPerPixel = 1.0f / pixelsPerUnit;
    glm::vec2 snappedPosition = position;
    if (snapToPixel)
    {
        snappedPosition.x = glm::trunc(snappedPosition.x / unitsPerPixel) * unitsPerPixel;
        snappedPosition.y = glm::trunc(snappedPosition.y / unitsPerPixel) * unitsPerPixel;
    }

    const glm::mat4 view = glm::lookAt(
        glm::vec3(snappedPosition, -1.0f),
        glm::vec3(snappedPosition, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    return view;
}

glm::mat4 Camera::GetProjection() const
{
    const float unitsPerPixel = 1.0f / pixelsPerUnit;

    const glm::mat4 projection = glm::ortho(
        windowDimensions.x * -0.5f * unitsPerPixel,
        windowDimensions.x * 0.5f * unitsPerPixel,
        windowDimensions.y * -0.5f * unitsPerPixel,
        windowDimensions.y * 0.5f * unitsPerPixel,
        0.0f,
        1.0f);

    return projection;
}

glm::mat4 Camera::GetViewProjection() const
{
    return GetProjection() * GetView();
}

glm::vec3 Camera::GetPosition() const
{
    return glm::vec3(position, -1.0f);
}

glm::vec3 Camera::GetForwardDirection() const
{
    return glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 Camera::GetUpDirection() const
{
    return glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec2 Camera::ScreenPointToWorld(u32 screenX, u32 screenY) const
{
    const glm::mat4 vp = GetViewProjection();
    const glm::mat4 vpInverse = glm::inverse(vp);

    const glm::vec2 ndc(
        (float)screenX / windowDimensions.x * 2.0f - 1.0f,
        (float)(windowDimensions.y - screenY) / windowDimensions.y * 2.0f - 1.0f);

    const glm::vec2 world =  vpInverse * glm::vec4(ndc, 0.0f, 1.0f);

    return world;
}

glm::vec2 Camera::WorldPointToScreen(glm::vec2 world) const
{
    const glm::mat4 vp = GetViewProjection();
    const glm::vec2 ndc = vp * glm::vec4(world, 0.0f, 1.0f);
    const glm::vec2 normalizedScreen = (ndc + glm::vec2(1.0f)) * glm::vec2(0.5f);
    const glm::vec2 screen(normalizedScreen.x * windowDimensions.x, (1.0f - normalizedScreen.y) * windowDimensions.y);

    return screen;
}

float Camera::PixelsToWorldUnits(float pixels) const
{
    const float unitsPerPixel = 1.0f / pixelsPerUnit;
    const float world = pixels * unitsPerPixel;
    return world;
}

float Camera::WorldUnitsToPixels(float worldUnits) const
{
    const float pixels = worldUnits * pixelsPerUnit;
    return pixels;
}

glm::vec2 Camera::PixelsToWorldUnits(glm::vec2 pixels) const
{
    const glm::vec2 world = { PixelsToWorldUnits(pixels.x), PixelsToWorldUnits(pixels.y) };
    return world;
}

glm::vec2 Camera::WorldUnitsToPixels(glm::vec2 worldUnits) const
{
    const glm::vec2 screen = { WorldUnitsToPixels(worldUnits.x), WorldUnitsToPixels(worldUnits.y) };
    return screen;
}

}
