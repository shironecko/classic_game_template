#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/game_state.h>
#include <examples/tower_defence/map_data.h>

void GameState::TimeStep(const MapData& mapData, const GameState& initial, GameState& next, const GameCommandQueue& commands, float delta)
{
    // clear next state and prepare it advancement
    next.enemies.clear();
    next.enemies.reserve(initial.enemies.size());
    next.towers.clear();
    next.towers.reserve(initial.towers.size());

    next.randomEngine = initial.randomEngine;

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

        const auto& enemyType = mapData.enemyTypes[enemy.type];

        const glm::vec2 a = enemyPath.waypoints[enemy.targetPointIdx - 1];
        const glm::vec2 b = enemyPath.waypoints[enemy.targetPointIdx];

        const glm::vec2 nextWaypointDirection = glm::normalize(b - enemy.position);

        const glm::vec2 closestPathPoint = cgt::math::AABBClosestPoint(
            enemy.position,
            glm::min(a, b),
            glm::max(a, b));

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
        enemyNext.position += enemy.direction * enemyType.speed * delta;
        enemyNext.position += pushbackFromOthers * enemyType.speed * flockPushbackFactor * delta;
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
        case GameCommand::Type::BuildTower:
        {
            auto& cmdData = command.data.buildTowerData;
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
    // TODO: more complete interpolation
    outState = nextState;

    for (u32 i = 0; i < prevState.enemies.size() && i < nextState.enemies.size(); ++i)
    {
        const Enemy& a = prevState.enemies[i];
        const Enemy& b = nextState.enemies[i];
        Enemy& result = outState.enemies[i];
        result.position = glm::lerp(a.position, b.position, factor);
        result.direction = glm::lerp(a.direction, b.direction, factor);
    }
}
