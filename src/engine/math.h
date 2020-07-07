#pragma once

namespace cgt::math
{

struct AABB
{
    static AABB FromPoints(glm::vec2 a, glm::vec2 b)
    {
        return { glm::min(a, b), glm::max(a, b) };
    }

    glm::vec2 min;
    glm::vec2 max;
};

inline glm::vec2 AABBClosestPoint(glm::vec2 point, AABB aabb)
{
    glm::vec2 closestPoint(
        glm::clamp(point.x, aabb.min.x, aabb.max.x),
        glm::clamp(point.y, aabb.min.y, aabb.max.y));

    return closestPoint;
}

inline float VectorAngle(glm::vec2 vector)
{
    float radians = glm::acos(vector.x) * glm::sign(vector.y);
    return glm::degrees(radians);
}

}