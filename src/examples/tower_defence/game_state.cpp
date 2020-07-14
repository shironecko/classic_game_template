#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/game_state.h>
#include <examples/tower_defence/entity_types.h>
#include <examples/tower_defence/helper_functions.h>

void GameState::TimeStep(const MapData& mapData, const GameState& initial, GameState& next, const GameCommandQueue& commands, GameEventQueue& outGameEvents, float delta)
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
    const float flockWaypointSteeringFactor = 1.0f;
    const float flockSightRange = 3.0f;
    const float flockRoadRecenteringStartDistance = 0.7f;
    const float flockRoadRecenteringMaxDistance = 1.0f;
    const float flockRoadRecenteringFactor = 5.5f;

    const float flockDesiredSpacing = 0.7f;
    const float flockMinimumSpacing = 0.4;
    const float flockPushbackFactor = 1.0f;

    const float flockCenteringFactor = 0.2f;

    const auto& enemyPath = mapData.enemyPath;
    for (const Enemy& enemy : initial.enemies)
    {
        Enemy& enemyNext = next.enemies.emplace_back();
        enemyNext = enemy;

        if (cgt::math::IsNearlyZero(enemy.remainingHealth)
            || cgt::math::IsNearlyZero(cgt::math::DistanceSqr(enemy.position, enemyPath.waypoints.back())))
        {
            next.enemies.pop_back();
            continue;
        }

        const glm::vec2 a = enemyPath.waypoints[enemy.nextWaypointIdx - 1];
        const glm::vec2 b = enemyPath.waypoints[enemy.nextWaypointIdx];
        const glm::vec2 targetDirection = glm::normalize(b - a);
        const glm::vec2 inverseTargetDirection = targetDirection * -1.0f;

        const glm::vec2 closestPathPoint = glm::closestPointOnLine(enemy.position, a, b);
        const float nextWaypointDistanceFromGoal = enemyPath.distancesToGoal[enemy.nextWaypointIdx];
        const float enemyDistanceFromGoal = nextWaypointDistanceFromGoal + glm::distance(closestPathPoint, b);
        enemyNext.distanceToGoal = enemyDistanceFromGoal;

        const float pathLookahead = 1.0f;
        if (cgt::math::DistanceSqr(closestPathPoint, b) < pathLookahead
            && enemy.nextWaypointIdx < enemyPath.waypoints.size() - 1)
        {
            ++enemyNext.nextWaypointIdx;
        }

        const auto& enemyType = mapData.enemyTypes[enemy.typeIdx];

        u32 othersInSight = 0;
        u32 othersInFrontThatAreTooClose = 0;
        glm::vec2 othersCumulativePosition(0.0f);
        glm::vec2 pushbackDirection(0.0f);
        for (const auto& otherEnemy : initial.enemies)
        {
            if (&enemy == &otherEnemy)
            {
                continue;
            }

            glm::vec2 fromOther = enemy.position - otherEnemy.position;
            float fromOtherDistSqr = cgt::math::LengthSqr(fromOther);
            if (fromOtherDistSqr > flockSightRange * flockSightRange)
            {
                continue;
            }

            if (otherEnemy.distanceToGoal < enemy.distanceToGoal)
            {
                ++othersInSight;
                othersCumulativePosition += otherEnemy.position;
            }

            if (fromOtherDistSqr < flockDesiredSpacing * flockDesiredSpacing)
            {
                if (otherEnemy.distanceToGoal < enemy.distanceToGoal)
                {
                    ++othersInFrontThatAreTooClose;
                }

                float fromOtherDist = glm::sqrt(fromOtherDistSqr);
                glm::vec2 fromOtherNorm = fromOther / fromOtherDist;
                float pushbackForce = 1.0f - glm::smoothstep(flockMinimumSpacing, flockDesiredSpacing, fromOtherDist);
                const glm::vec2 pushbackVector = fromOtherNorm * pushbackForce;
                pushbackDirection += pushbackVector;
            }
        }

        glm::vec2 roadRecenteringDirection = closestPathPoint - enemy.position;
        const float distanceFromTheRoadCenter = glm::length(roadRecenteringDirection);
        const float roadRecenteringForce = glm::smoothstep(flockRoadRecenteringStartDistance, flockRoadRecenteringMaxDistance, distanceFromTheRoadCenter);
        roadRecenteringDirection = glm::normalize(roadRecenteringDirection) * roadRecenteringForce;

        glm::vec2 recenteringDirection(0.0f);
        if (othersInSight > 0)
        {
            glm::vec2 othersAveragePosition = othersCumulativePosition / (float)othersInSight;
            recenteringDirection = glm::normalize(othersAveragePosition - enemy.position);
        }

        const float accelerationFactor = 10.0f;
        const float acceleration = enemyType.speed * accelerationFactor;

        enemyNext.velocity += targetDirection * flockWaypointSteeringFactor * acceleration * delta;
        enemyNext.velocity += recenteringDirection * flockCenteringFactor * acceleration * delta;
        enemyNext.velocity += roadRecenteringDirection * flockRoadRecenteringFactor * acceleration * delta;

        const float velocityLength = glm::length(enemyNext.velocity);
        const glm::vec2 velocityNormalized = enemyNext.velocity / velocityLength;
        if (velocityLength > enemyType.speed)
        {
            enemyNext.velocity = velocityNormalized * enemyType.speed;
        }

        enemyNext.velocity += pushbackDirection * flockPushbackFactor * acceleration * delta;

        enemyNext.rotation = cgt::math::VectorAngle(velocityNormalized);
        enemyNext.position += enemyNext.velocity * delta;
    }

    // towers update
    static std::vector<u32> enemyQueryStorage;
    for (const Tower& tower : initial.towers)
    {
        Tower& towerNext = next.towers.emplace_back();
        towerNext = tower;

        const TowerType& type = mapData.towerTypes[tower.typeIdx];
        enemyQueryStorage.clear();
        QueryEnemiesInRadius(next.enemies, towerNext.position, type.range, enemyQueryStorage);
        if (enemyQueryStorage.empty())
        {
            continue;
        }

        std::sort(enemyQueryStorage.begin(), enemyQueryStorage.end(), [&](u32 aIdx, u32 bIdx) {
            const Enemy& a = next.enemies[aIdx];
            const Enemy& b = next.enemies[bIdx];
            return a.distanceToGoal < b.distanceToGoal;
        });

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
            newProjectile.typeIdx = type.projectileTypeIdx;
            newProjectile.position = tower.position;
            newProjectile.targetEnemyIndex = targetEnemyIdx;
            newProjectile.targetEnemyId = targetEnemy.id;
            newProjectile.lastEnemyPosition = targetEnemy.position;
            newProjectile.rotation = towerNext.rotation;

            auto& event = outGameEvents.emplace_back();
            event.type = GameEvent::Type::ProjectileLaunched;
            auto& eventData = event.data.projectileLaunchedData;
            eventData.typeIdx = newProjectile.typeIdx;
            eventData.position = newProjectile.position;

            // TODO: advance projectiles
        }
    }

    // projectiles update
    auto applyDamageToEnemy = [&](Enemy& enemy, const Projectile& projectile, const ProjectileType& projectileType) {
        const bool enemyDied = enemy.remainingHealth <= projectileType.damage;
        enemy.remainingHealth = glm::max(0.0f, enemy.remainingHealth - projectileType.damage);

        auto& event = outGameEvents.emplace_back();
        event.type = GameEvent::Type::ProjectileHit;
        auto& eventData = event.data.projectileHitData;
        eventData.projectileTypeIdx = projectile.typeIdx;
        eventData.position = enemy.position;
        eventData.enemyIndex = projectile.targetEnemyIndex;

        if (enemyDied)
        {
            const EnemyType& enemyType = mapData.enemyTypes[enemy.typeIdx];
            next.playerState.gold += enemyType.goldReward;

            auto& event = outGameEvents.emplace_back();
            event.type = GameEvent::Type::EnemyDied;
            auto& eventData = event.data.enemyDiedData;
            eventData.enemyIndex = projectile.targetEnemyIndex;
        }

    };

    for (u32 i = 0; i < initial.projectiles.size(); ++i)
    {
        const Projectile& proj = initial.projectiles[i];
        Projectile projNext = proj;
        if (next.enemies.size() <= projNext.targetEnemyIndex)
        {
            continue;
        }

        bool targetEnemyAlive =
            next.enemies.size() > projNext.targetEnemyIndex
            && next.enemies[projNext.targetEnemyIndex].id == projNext.targetEnemyId;

        const glm::vec2 targetPosition = targetEnemyAlive
            ? next.enemies[projNext.targetEnemyIndex].position
            : projNext.lastEnemyPosition;

        projNext.lastEnemyPosition = targetPosition;
        glm::vec2 toTarget = targetPosition - projNext.position;
        float toTargetDst = glm::length(toTarget);
        glm::vec2 toTargetNorm = toTarget / toTargetDst;
        projNext.rotation = cgt::math::VectorAngle(toTargetNorm);

        const ProjectileType& projectileType = mapData.projectileTypes[projNext.typeIdx];
        float stepDst = projectileType.speed * delta;

        if (toTargetDst > stepDst)
        {
            glm::vec2 stepVec = toTargetNorm * projectileType.speed * delta;
            projNext.position += stepVec;
            next.projectiles.emplace_back(projNext);
        }
        else
        {
            if (targetEnemyAlive)
            {
                Enemy& target = next.enemies[projNext.targetEnemyIndex];
                applyDamageToEnemy(target, projNext, projectileType);
            }

            if (!cgt::math::IsNearlyZero(projectileType.splashRadius))
            {
                enemyQueryStorage.clear();
                QueryEnemiesInRadius(next.enemies, targetPosition, projectileType.splashRadius, enemyQueryStorage);
                for (u32 enemyIndex : enemyQueryStorage)
                {
                    Enemy& enemy = next.enemies[enemyIndex];
                    applyDamageToEnemy(enemy, projNext, projectileType);
                }
            }
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
        result.rotation = cgt::math::AngleLerp(a.rotation, b.rotation, factor);
        result.remainingHealth = b.remainingHealth;
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
        result.rotation = cgt::math::AngleLerp(a.rotation, b.rotation, factor);
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
        if (cgt::math::IsNearlyZero(enemy.remainingHealth))
        {
            continue;
        }

        const glm::vec2 x = enemy.position - position;
        const float distanceSqr = glm::dot(x, x);
        if (distanceSqr <= radiusSqr)
        {
            outResults.emplace_back(i);
        }
    }
}

void GameState::ForEachEntity(const MapData& mapData, std::function<void(const Entity&, const EntityType&)> function) const
{
    for (const auto& enemy : enemies)
    {
        const auto& type = mapData.enemyTypes[enemy.typeIdx];
        function(enemy, type);
    }

    for (const auto& tower : towers)
    {
        const auto& type = mapData.towerTypes[tower.typeIdx];
        function(tower, type);
    }

    for (const auto& projectile : projectiles)
    {
        const auto& type = mapData.projectileTypes[projectile.typeIdx];
        function(projectile, type);
    }
}

void GameState::ForEachEnemy(const MapData& mapData, std::function<void(const Enemy&, const EnemyType&)> function) const
{
    for (const auto& enemy : enemies)
    {
        const auto& type = mapData.enemyTypes[enemy.typeIdx];
        function(enemy, type);
    }
}
