#pragma once

struct Enemy
{
    typedef u32 TypeId;

    TypeId type = 0;

    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 direction = glm::vec2(0.0f);
    u32 targetPointIdx = 0;

    float remainingHealth = 0.0f;
};

struct EnemyType
{
    float maxHealth = 0.0f;
    float speed = 0.0f;
    float goldReward = 0.0f;
    u32 unitsPerSpawn = 0;
    u32 tileId = 0;

    std::string name;
};

typedef std::vector<EnemyType> EnemyTypeCollection;

class EnemyPath;

void LoadEnemyTypes(tson::Map& map, EnemyTypeCollection& outEnemyTypes);
void SetupEnemy(const EnemyTypeCollection& enemyTypes, Enemy::TypeId typeId, const EnemyPath& path, Enemy& outEnemy);
