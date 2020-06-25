#pragma once

namespace cgt::math
{

inline glm::vec2 AABBClosestPoint(glm::vec2 point, glm::vec2 aabbMin, glm::vec2 aabbMax)
{
    glm::vec2 closestPoint(
        glm::clamp(point.x, aabbMin.x, aabbMax.x),
        glm::clamp(point.y, aabbMin.y, aabbMax.y));

    return closestPoint;
}

inline float VectorAngle(glm::vec2 vector)
{
    float radians = glm::acos(vector.x) * glm::sign(vector.y);
    return glm::degrees(radians);
}

}