#pragma once

#include <glm/mat4x4.hpp>

namespace cgt::render
{

class ICamera
{
public:
    virtual glm::mat4 GetView() const = 0;
    virtual glm::mat4 GetProjection() const = 0;
    virtual glm::mat4 GetViewProjection() const = 0;
    virtual glm::vec3 GetPosition() const = 0;
    virtual glm::vec3 GetForwardDirection() const = 0;
    virtual glm::vec3 GetUpDirection() const = 0;

    virtual glm::vec2 ScreenToWorld(u32 screenX, u32 screenY) const = 0;

    virtual bool IsOrthographic() const = 0;
};

}