#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/game_state.h>
#include <examples/tower_defence/entity_types.h>

void GameState::TimeStep(const MapData& mapData, const GameState& initial, GameState& next, const GameCommandQueue& commands, float delta)
{
    ZoneScoped;

    // clear next state and prepare it advancement
    next.enemies.clear();
    next.enemies.reserve(initial.enemies.size());
    next.towers.clear();
    next.towers.reserve(initial.towers.size());
    next.projectiles.clear();
    next.projectiles.reserve(initial.projectiles.size());

    next.playerState = initial.playerState;
    next.randomEngine = initial.randomEngine;
    next.nextObjectId = initial.nextObjectId;

    // enemy movement system
    static float flockSteeringSpeed = 12.0f;
    static float flockWaypointSteeringFactor = 1.0f;
    static float flockCommonDirectionFactor = 0.2f;

    static float flockSightRange = 3.0f;

    static float flockMaxPathDistance = 0.8f;
    static float flockPathReadjustFactor = 0.4f;

    static float flockDesiredSpacing = 0.7f;
    static float flockMinimumSpacing = 0.4;
    static float flockPushbackFactor = 1.0f;

    static float flockCenteringFactor = 0.3f;

    const auto& enemyPath = mapData.enemyPath;
    for (const Enemy& enemy : initial.enemies)
    {
        Enemy& enemyNext = next.enemies.emplace_back();
        enemyNext = enemy;

        if (enemy.targetPointIdx >= enemyPath.waypoints.size())
        {
            continue;
        }

        if (cgt::math::IsNearlyZero(enemy.remainingHealth))
        {
            next.enemies.pop_back();
            continue;
        }

        const auto& enemyType = mapData.enemyTypes[enemy.typeIdx];

        const glm::vec2 a = enemyPath.waypoints[enemy.targetPointIdx - 1];
        const glm::vec2 b = enemyPath.waypoints[enemy.targetPointIdx];

        const glm::vec2 nextWaypointDirection = glm::normalize(b - enemy.position);

        const glm::vec2 closestPathPoint = cgt::math::AABBClosestPoint(
            enemy.position,
            cgt::math::AABB::FromPoints(a, b));

        glm::vec2 pathReadjust(0.0f);
        glm::vec2 toClosestPathPoint = closestPathPoint - enemy.position;
        float toClosestPathPointDistSqr = glm::dot(toClosestPathPoint, toClosestPathPoint);
        if (toClosestPathPointDistSqr > 0.0f)
        {
            pathReadjust = glm::normalize(closestPathPoint - enemy.position);
        }

        if (glm::distance(closestPathPoint, b) < 0.1f)
        {
            ++enemyNext.targetPointIdx;
        }

        u32 othersInSight = 0;
        glm::vec2 othersCumulativePosition(0.0f);
        glm::vec2 othersCumulativeDirection(0.0f);
        glm::vec2 pushbackFromOthers(0.0f);
        for (const auto& otherEnemy : initial.enemies)
        {
            if (&enemy == &otherEnemy)
            {
                continue;
            }

            glm::vec2 fromOther = enemy.position - otherEnemy.position;
            float fromOtherDistSqr = glm::dot(fromOther, fromOther);
            if (fromOtherDistSqr > flockSightRange * flockSightRange)
            {
                continue;
            }

            ++othersInSight;

            othersCumulativePosition += otherEnemy.position;
            othersCumulativeDirection += otherEnemy.direction;

            if (fromOtherDistSqr < flockDesiredSpacing * flockDesiredSpacing)
            {
                float fromOtherDist = glm::sqrt(fromOtherDistSqr);
                glm::vec2 fromOtherNorm = fromOther / fromOtherDist;
                float pushbackForce = 1.0f - glm::smoothstep(flockMinimumSpacing, flockDesiredSpacing, fromOtherDist);
                pushbackFromOthers += fromOtherNorm * pushbackForce;
            }
        }

        glm::vec2 directionDelta = nextWaypointDirection * flockWaypointSteeringFactor;
        if (othersInSight > 0)
        {
            glm::vec2 othersAveragePosition = othersCumulativePosition / (float)othersInSight;
            glm::vec2 centeringDirection = glm::normalize(othersAveragePosition - enemy.position);
            directionDelta += centeringDirection * flockCenteringFactor;

            directionDelta += glm::normalize(othersCumulativeDirection) * flockCommonDirectionFactor;

            //directionDelta += pushbackFromOthers;
        }

        directionDelta += pathReadjust * flockPathReadjustFactor;

        enemyNext.direction = glm::normalize(enemy.direction + directionDelta * flockSteeringSpeed * delta);
        enemyNext.rotation = cgt::math::VectorAngle(enemyNext.direction);
        enemyNext.position += enemy.direction * enemyType.speed * delta;
        enemyNext.position += pushbackFromOthers * enemyType.speed * flockPushbackFactor * delta;
    }

    // towers update
    static std::vector<u32> enemyQueryStorage;
    for (const Tower& tower : initial.towers)
    {
        // TODO: actual update
        Tower& towerNext = next.towers.emplace_back();
        towerNext = tower;

        const TowerType& type = mapData.towerTypes[tower.typeIdx];
        enemyQueryStorage.clear();
        QueryEnemiesInRadius(next.enemies, towerNext.position, type.range, enemyQueryStorage);
        if (enemyQueryStorage.empty())
        {
            continue;
        }

        // TODO: target closest to the goal enemy
        const u32 targetEnemyIdx = *enemyQueryStorage.begin();
        const Enemy& targetEnemy = next.enemies[targetEnemyIdx];
        const glm::vec2 toEnemy = glm::normalize(targetEnemy.position - towerNext.position);
        towerNext.rotation = cgt::math::VectorAngle(toEnemy);

        towerNext.timeSinceLastShot += delta;
        const float shotInterval = 1.0f / type.shotsPerSecond;
        while (towerNext.timeSinceLastShot > shotInterval)
        {
            towerNext.timeSinceLastShot -= shotInterval;
            Projectile& newProjectile = next.projectiles.emplace_back();
            newProjectile.id = next.nextObjectId++;
            newProjectile.typeIdx = tower.typeIdx;
            newProjectile.position = tower.position;
            newProjectile.targetEnemyId = targetEnemyIdx;

            // TODO: advance projectiles
        }
    }

    // projectiles update
    for (u32 i = 0; i < initial.projectiles.size(); ++i)
    {
        const Projectile& proj = initial.projectiles[i];
        Projectile projNext = proj;
        if (next.enemies.size() <= projNext.targetEnemyId)
        {
            continue;
        }

        Enemy& target = next.enemies[projNext.targetEnemyId];
        glm::vec2 toTarget = target.position - projNext.position;
        float toTargetDst = glm::length(toTarget);
        glm::vec2 toTargetNorm = toTarget / toTargetDst;
        projNext.rotation = cgt::math::VectorAngle(toTargetNorm);

        const TowerType& parentTowerType = mapData.towerTypes[projNext.typeIdx];
        float stepDst = parentTowerType.projectileSpeed * delta;

        if (toTargetDst > stepDst)
        {
            glm::vec2 stepVec = toTargetNorm * parentTowerType.projectileSpeed * delta;
            projNext.position += stepVec;
            next.projectiles.emplace_back(projNext);
        }
        else
        {
            target.remainingHealth = glm::max(0.0f, target.remainingHealth - parentTowerType.damage);
        }
    }

    // game commands execution
    for (auto& command : commands)
    {
        switch (command.type)
        {
        case GameCommand::Type::Debug_SpawnEnemy:
        {
            auto& cmdData = command.data.debug_spawnEnemyData;
            const EnemyType& enemyType = mapData.enemyTypes[cmdData.enemyType];

            Enemy& enemy = next.enemies.emplace_back();
            enemy.id = next.nextObjectId++;
            SetupEnemy(mapData.enemyTypes, cmdData.enemyType, mapData.enemyPath, enemy);

            std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
            glm::vec2 randomShift(
                distribution(next.randomEngine),
                distribution(next.randomEngine));
            enemy.position += randomShift;
            break;
        }
        case GameCommand::Type::Debug_DespawnAllEnemies:
        {
            next.enemies.clear();
            break;
        }
        case GameCommand::Type::Debug_AddGold:
        {
            auto& cmdData = command.data.debug_addGoldData;
            next.playerState.gold += cmdData.amount;
            break;
        }
        case GameCommand::Type::BuildTower:
        {
            auto& cmdData = command.data.buildTowerData;
            const TowerType& type = mapData.towerTypes[cmdData.towerType];
            if (next.playerState.gold >= type.cost)
            {
                next.playerState.gold -= type.cost;
                Tower& newTower = next.towers.emplace_back();
                newTower.id = next.nextObjectId++;
                SetupTower(mapData.towerTypes, cmdData.towerType, cmdData.position, newTower);
            }
            break;
        }
        default:
        {
            CGT_PANIC("Unsupported command type!");
            break;
        }
        }
    }
}

void GameState::Interpolate(const GameState& prevState, const GameState& nextState, GameState& outState, float factor)
{
    ZoneScoped;

    // TODO: more complete interpolation
    outState.playerState = nextState.playerState;
    outState.towers.clear();
    outState.projectiles.clear();
    outState.enemies.clear();

    // enemies
    for (u32 i = 0, j = 0; i < prevState.enemies.size() && j < nextState.enemies.size(); ++i, ++j)
    {
        const Enemy& a = prevState.enemies[i];
        const Enemy& b = nextState.enemies[j];
        if (a.id != b.id)
        {
            --j;
            continue;
        }

        Enemy& result = outState.enemies.emplace_back();
        result = b;
        result.position = glm::lerp(a.position, b.position, factor);
        result.rotation = glm::lerp(a.rotation, b.rotation, factor);

        result.direction = glm::lerp(a.direction, b.direction, factor);
        result.remainingHealth = glm::lerp(a.remainingHealth, b.remainingHealth, factor);
    }

    // towers
    for (u32 i = 0, j = 0; i < prevState.towers.size() && j < nextState.towers.size(); ++i, ++j)
    {
        const Tower& a = prevState.towers[i];
        const Tower& b = nextState.towers[j];
        if (a.id != b.id)
        {
            --j;
            continue;
        }

        Tower& result = outState.towers.emplace_back();
        result = b;
        result.rotation = glm::lerp(a.rotation, b.rotation, factor);
    }

    // projectiles
    for (u32 i = 0, j = 0; i < prevState.projectiles.size() && j < nextState.projectiles.size(); ++i, ++j)
    {
        const Projectile& a = prevState.projectiles[i];
        const Projectile& b = nextState.projectiles[j];
        if (a.id != b.id)
        {
            --j;
            continue;
        }

        Projectile& result = outState.projectiles.emplace_back();
        result = b;
        result.position = glm::lerp(a.position, b.position, factor);
        result.rotation = glm::lerp(a.rotation, b.rotation, factor);
    }
}

void GameState::QueryEnemiesInRadius(const std::vector<Enemy>& enemies, glm::vec2 position, float radius, std::vector<u32>& outResults)
{
    const float radiusSqr = radius * radius;
    for (u32 i = 0; i < enemies.size(); ++i)
    {
        const Enemy& enemy = enemies[i];
        const glm::vec2 x = enemy.position - position;
        const float distanceSqr = glm::dot(x, x);
        if (distanceSqr <= radiusSqr)
        {
            outResults.emplace_back(i);
        }
    }
}
