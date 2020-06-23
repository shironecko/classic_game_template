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

glm::mat4 CameraSimpleOrtho::GetView() const
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

glm::mat4 CameraSimpleOrtho::GetProjection() const
{
    const float unitsPerPixel = 1.0f / pixelsPerUnit;

    const glm::mat4 projection = glm::ortho(
        windowWidth * -0.5f * unitsPerPixel,
        windowWidth * 0.5f * unitsPerPixel,
        windowHeight * -0.5f * unitsPerPixel,
        windowHeight * 0.5f * unitsPerPixel,
        0.0f,
        1.0f);

    return projection;
}

glm::mat4 CameraSimpleOrtho::GetViewProjection() const
{
    return GetProjection() * GetView();
}

glm::vec3 CameraSimpleOrtho::GetPosition() const
{
    return glm::vec3(position, -1.0f);
}

glm::vec3 CameraSimpleOrtho::GetForwardDirection() const
{
    return glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 CameraSimpleOrtho::GetUpDirection() const
{
    return glm::vec3(0.0f, 1.0f, 0.0f);
}

bool CameraSimpleOrtho::IsOrthographic() const
{
    return true;
}

}
