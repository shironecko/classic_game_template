#include <render_core/pch.h>

#include <render_core/camera_simple_ortho.h>
#include <engine/window.h>

namespace cgt::render
{
CameraSimpleOrtho::CameraSimpleOrtho(const Window& window)
{
    windowWidth = window.GetWidth();
    windowHeight = window.GetHeight();
}

glm::mat4 CameraSimpleOrtho::GetViewProjection() const
{
    glm::vec2 snappedPosition = position;
    const float unitsPerPixel = 1.0f / pixelsPerUnit;
    if (snapToPixel)
    {
        snappedPosition.x = glm::trunc(snappedPosition.x / unitsPerPixel) * unitsPerPixel;
        snappedPosition.y = glm::trunc(snappedPosition.y / unitsPerPixel) * unitsPerPixel;
    }

    const glm::mat4 view = glm::lookAt(
        glm::vec3(snappedPosition, -1.0f),
        glm::vec3(snappedPosition, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    const glm::mat4 projection = glm::ortho(
        windowWidth * -0.5f * unitsPerPixel,
        windowWidth * 0.5f * unitsPerPixel,
        windowHeight * -0.5f * unitsPerPixel,
        windowHeight * 0.5f * unitsPerPixel,
        0.0f,
        1.0f);

    return projection * view;
}

}
