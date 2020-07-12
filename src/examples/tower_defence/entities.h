#pragma once

struct Entity
{
    u32 id = 0;
    u32 typeIdx = 0;

    glm::vec2 position = glm::vec2(0.0f);
    float rotation = 0.0f;
};

struct Enemy : Entity
{
    glm::vec2 direction = glm::vec2(0.0f);
    u32 targetPointIdx = 0;
    float remainingHealth = 0.0f;
};

struct Tower : Entity
{
    float timeSinceLastShot = 0.0f;
};

struct Projectile : Entity
{
    u32 targetEnemyId = 0;
};
