#pragma once

struct Projectile
{
    glm::vec2 position = glm::vec2(0.0f);
    float rotation = 0.0f;

    u32 parentTowerTypeId = 0;
    u32 targetEnemyId = 0;
};


