#pragma once

struct Enemy
{
    u32 id = 0;
    u32 typeIdx = 0;

    glm::vec2 position = glm::vec2(0.0f);
    float rotation = 0.0f;

    glm::vec2 direction = glm::vec2(0.0f);
    u32 targetPointIdx = 0;
    float remainingHealth = 0.0f;
};

struct Tower
{
    u32 id = 0;
    u32 typeIdx = 0;

    glm::vec2 position = glm::vec2(0.0f);
    float rotation = 0.0f;

    float timeSinceLastShot = 0.0f;
};

struct Projectile
{
    u32 id = 0;
    u32 typeIdx = 0;

    glm::vec2 position = glm::vec2(0.0f);
    float rotation = 0.0f;

    u32 targetEnemyId = 0;
};
