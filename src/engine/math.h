#pragma once

namespace cgt::math
{

/*
 * Credit for float comparing functions: https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
 */

constexpr float DEFAULT_MAX_REL_DIFF = std::numeric_limits<float>::epsilon();
constexpr float DEFAULT_MAX_DIFF = DEFAULT_MAX_REL_DIFF * 2.0f;
constexpr u32 DEFAULT_MAX_ULPS = 2;

inline i32 ReinterpretAsInt(float f)
{
    return *reinterpret_cast<i32*>(&f);
}

inline float ReinterpretAsFloat(i32 i)
{
    return *reinterpret_cast<float*>(&i);
}

inline bool IsNearlyZero(float f, float max_diff = DEFAULT_MAX_DIFF)
{
    return f <= max_diff;
}

// Check if the numbers are really close -- needed
// when comparing numbers near zero.
inline bool AreNearlyEqAbs(float a, float b, float max_diff = DEFAULT_MAX_DIFF)
{
    const float abs_diff = std::fabsf(a - b);
    return IsNearlyZero(abs_diff, max_diff);
}

inline bool AreNearlyEqUlps(float a, float b, u32 max_ulps_diff = DEFAULT_MAX_ULPS)
{
    // Different signs means they do not match.
    if ((a < 0.0f) != (b < 0.0f)) {
        return false;
    }

    const i32 a_int = ReinterpretAsInt(a);
    const i32 b_int = ReinterpretAsInt(b);

    // Find the difference in ULPs.
    const u32 ulps_diff = std::abs(a_int - b_int);
    return ulps_diff <= max_ulps_diff;
}

inline bool AreNearlyEqRel(float a, float b, float max_rel_diff = DEFAULT_MAX_REL_DIFF)
{
    const float a_abs = std::fabsf(a);
    const float b_abs = std::fabsf(b);
    const float largest = std::max(a_abs, b_abs);

    const float diff = std::fabsf(a - b);
    return diff <= largest * max_rel_diff;
}

inline bool AreNearlyEqAbsUlps(float a, float b, float max_diff = DEFAULT_MAX_DIFF, u32 max_ulps_diff = DEFAULT_MAX_ULPS)
{
    return AreNearlyEqAbs(a, b, max_diff) || AreNearlyEqUlps(a, b, max_ulps_diff);
}

inline bool AreNearlyEqAbsRel(float a, float b, float max_diff = DEFAULT_MAX_DIFF, float max_rel_diff = DEFAULT_MAX_REL_DIFF)
{
    return AreNearlyEqAbs(a, b, max_diff) || AreNearlyEqRel(a, b, max_rel_diff);
}

inline float WorldToPixels(float world, float pixelsPerUnit)
{
    const float pixels = world * pixelsPerUnit;
    return pixels;
}

inline float PixelsToWorld(float pixels, float pixelsPerUnit)
{
    const float unitsPerPixel = 1.0f / pixelsPerUnit;
    const float world = pixels * unitsPerPixel;
    return world;
}

inline glm::vec2 WorldToPixels(glm::vec2 world, float pixelsPerUnit)
{
    const glm::vec2 pixels(
        WorldToPixels(world.x, pixelsPerUnit),
        WorldToPixels(world.y, pixelsPerUnit));

    return pixels;
}

inline glm::vec2 PixelsToWorld(glm::vec2 pixels, float pixelsPerUnit)
{
    const glm::vec2 world(
        PixelsToWorld(pixels.x, pixelsPerUnit),
        PixelsToWorld(pixels.y, pixelsPerUnit));

    return world;
}

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