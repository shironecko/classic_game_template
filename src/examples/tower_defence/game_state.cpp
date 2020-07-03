#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/game_state.h>
#include <examples/tower_defence/map_data.h>

void GameState::TimeStep(const MapData& mapData, const GameState& initial, GameState& next, float delta)
{
    // clear next state and prepare it advancement
    next.enemies.clear();
    next.enemies.reserve(initial.enemies.size());
    next.towers.clear();
    next.towers.reserve(initial.towers.size());

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
    {
        ImGui::Begin("Flocking Options");

        ImGui::DragFloat("Steering Speed", &flockSteeringSpeed, 0.1f, 0.0f, 40.0f);
        ImGui::Spacing();

        ImGui::DragFloat("Sight Range", &flockSightRange, 0.1f, 0.0f, 5.0f);
        ImGui::DragFloat("Max Path Distance", &flockMaxPathDistance, 0.1f, 0.0f, 5.0f);
        ImGui::DragFloat("Desired Spacing", &flockDesiredSpacing, 0.1f, 0.0f, 5.0f);
        ImGui::DragFloat("Minimum Spacing", &flockMinimumSpacing, 0.1f, 0.0f, 5.0f);

        ImGui::TextUnformatted("Factors");
        ImGui::DragFloat("Waypoint", &flockWaypointSteeringFactor, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat("Common Direction", &flockCommonDirectionFactor, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat("Path Readjust", &flockPathReadjustFactor, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat("Pushback", &flockPushbackFactor, 0.1f, 0.0f, 2.0f);
        ImGui::DragFloat("Centering", &flockCenteringFactor, 0.1f, 0.0f, 1.0f);

        ImGui::End();
    }

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
}

void GameState::Interpolate(const GameState& prevState, const GameState& nextState, GameState& outState, float factor)
{
    // TODO: actual interpolation
    outState = prevState;
}
