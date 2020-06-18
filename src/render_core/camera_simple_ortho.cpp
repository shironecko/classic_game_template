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
    const glm::mat4 view = glm::lookAt(
        glm::vec3(position, -1.0f),
        glm::vec3(position, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    const float viewScale = 1.0f / pixelsPerUnit;
    const glm::mat4 projection = glm::ortho(
        windowWidth * -0.5f * viewScale,
        windowWidth * 0.5f * viewScale,
        windowHeight * -0.5f * viewScale,
        windowHeight * 0.5f * viewScale,
        0.0f,
        1.0f);

    return projection * view;
}

}
