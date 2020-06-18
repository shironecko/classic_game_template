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

    glm::mat4 GetViewProjection() const override;

    float pixelsPerUnit = 1.0f;

    glm::vec2 position = glm::vec2(0.0f);

private:
    float windowWidth;
    float windowHeight;
};

}
