#pragma once

struct Enemy
{
    typedef u32 TypeId;

    TypeId type;

    glm::vec2 position;
    glm::vec2 direction;
    u32 targetPointIdx;

    float remainingHealth;
};

struct EnemyType
{
    float maxHealth;
    float speed;
    float goldReward;
    u32 unitsPerSpawn;
    u32 tileId;

    std::string name;
};

typedef std::vector<EnemyType> EnemyTypeCollection;

class EnemyPath;

void LoadEnemyTypes(tson::Map& map, EnemyTypeCollection& outEnemyTypes);
void SetupEnemy(const EnemyTypeCollection& enemyTypes, Enemy::TypeId typeId, const EnemyPath& path, Enemy& outEnemy);
