#pragma once

#include <glm/mat4x4.hpp>

namespace cgt::render
{

class ICamera
{
public:
    virtual glm::mat4 GetViewProjection() const = 0;
};

}