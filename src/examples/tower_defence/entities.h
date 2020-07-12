#pragma once

struct Enemy
{
    typedef uint32_t TypeId;

    uint32_t id = 0;
    TypeId typeIdx = 0;

    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 direction = glm::vec2(0.0f);
    uint32_t targetPointIdx = 0;

    float remainingHealth = 0.0f;
};

struct Tower
{
    typedef u32 TypeId;

    u32 id = 0;
    TypeId typeIdx = 0;
    glm::vec2 position = glm::vec2(0.0f);
    float rotation = 0.0f;
    float timeSinceLastShot = 0.0f;
};

struct Projectile
{
    u32 id = 0;
    glm::vec2 position = glm::vec2(0.0f);
    float rotation = 0.0f;

    u32 parentTowerTypeId = 0;
    u32 targetEnemyId = 0;
};
