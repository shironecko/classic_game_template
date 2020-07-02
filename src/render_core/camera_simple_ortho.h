#pragma once

#include <render_core/i_camera.h>

namespace cgt
{
class Window;
}

namespace cgt::render
{

class CameraSimpleOrtho : public ICamera
{
public:
    CameraSimpleOrtho(const Window& window);

    glm::mat4 GetView() const override;
    glm::mat4 GetProjection() const override;
    glm::mat4 GetViewProjection() const override;
    glm::vec3 GetPosition() const override;
    glm::vec3 GetForwardDirection() const override;
    glm::vec3 GetUpDirection() const override;

    glm::vec2 ScreenToWorld(u32 screenX, u32 screenY) const override;

    bool IsOrthographic() const override;

    float pixelsPerUnit = 1.0f;
    bool snapToPixel = true;

    glm::vec2 position = glm::vec2(0.0f);

private:
    float windowWidth;
    float windowHeight;
};

}
