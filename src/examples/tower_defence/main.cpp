#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/tilemap.h>

struct EnemyPath
{
    std::string debugName;
    glm::vec4 debugColor;
    std::vector<glm::vec2> waypoints;

    void DebugRender()
    {
        Im3d::PushAlpha(0.5f);
        Im3d::PushColor({ debugColor.r, debugColor.g, debugColor.b, debugColor.a });

        Im3d::Text(glm::vec3(waypoints[0], 0.0f), Im3d::TextFlags_AlignTop, debugName.c_str());

        Im3d::PushSize(3.0f);

        Im3d::BeginLineStrip();
        for (auto point : waypoints)
        {
            Im3d::Vertex(glm::vec3(point, 0.0f));
        }
        Im3d::End();

        Im3d::PopSize();
        Im3d::PopColor();
        Im3d::PopAlpha();
    }
};

struct Enemy
{
    glm::vec2 position;
    TileSet::TileId tileId;

    std::string name;
    float health;
    float speed;
    float armor;
    u32 unitsPerSpawn;

    u32 targetPointIdx = 1;
};

int GameMain()
{
    auto window = cgt::WindowConfig::Default()
        .WithTitle("Tower Defence")
        .Build();

    auto render = cgt::render::RenderConfig::Default(window)
        .Build();

    auto imguiHelper = cgt::ImGuiHelper::Create(window, render);

    const float basePPU = 64.0f;
    const float SCALE_FACTORS[] = { 4.0f, 3.0f, 2.0f, 1.0f, 1.0f / 2.0f, 1.0f / 3.0f, 1.0f / 4.0f, 1.0f / 5.0f, 1.0f / 6.0f, 1.0f / 7.0f, 1.0f / 8.0f, 1.0f / 9.0f, 1.0f / 10.0f };
    i32 scaleFactorIdx = 3;

    cgt::render::CameraSimpleOrtho camera(*window);
    camera.pixelsPerUnit = 64.0f;

    tson::Tileson mapParser;
    tson::Map map = mapParser.parse(cgt::AssetPath("examples/maps/tower_defense.json"));
    CGT_ASSERT_ALWAYS(map.getStatus() == tson::ParseStatus::OK);

    tson::Tileset* rawTileset = map.getTileset("tower_defense");
    CGT_ASSERT_ALWAYS(rawTileset);

    cgt::render::TextureHandle tilesetTexture = render->LoadTexture(
        cgt::AssetPath("examples/maps") / rawTileset->getImagePath());

    auto tileset = TileSet::Load(*rawTileset, tilesetTexture);
    auto baseMapLayer = StaticTileGrid::Load(map, *map.getLayer("Base"), *rawTileset, 0);
    auto propsMapLayer = StaticTileGrid::Load(map, *map.getLayer("Props"), *rawTileset, 1);

    std::vector<EnemyPath> enemyPaths;
    auto* pathLayer = map.getLayer("Paths");
    CGT_ASSERT_ALWAYS(pathLayer && pathLayer->getType() == tson::LayerType::ObjectGroup);

    for (auto& object : pathLayer->getObjectsByType(tson::ObjectType::Polyline))
    {
        auto& path = enemyPaths.emplace_back();
        path.debugName = object.getName();
        auto color = object.get<tson::Colori>("PlayerColor").asFloat();
        path.debugColor = { color.r, color.g, color.b, color.a };

        glm::vec3 basePosition(
            (float)object.getPosition().x / map.getTileSize().x - 0.5f,
            (float)object.getPosition().y / map.getTileSize().y * -1.0f + 0.5f,
            0.0f);

        glm::mat4 baseRotation = glm::rotate(
            glm::mat4(1.0f),
            glm::radians(object.getRotation()),
            { 0.0f, 0.0f, 1.0f });

        for (auto& point: object.getPolylines())
        {
            glm::vec3 pointPosition(
                (float)point.x / map.getTileSize().x,
                (float)point.y / map.getTileSize().y * -1.0f,
                0.0f);

            glm::vec3 finalPosition = baseRotation * glm::vec4(pointPosition, 1.0f);
            finalPosition += basePosition;

            path.waypoints.emplace_back(finalPosition);
        }
    }

    std::vector<Enemy> enemyTypes;
    auto* enemyLayer = map.getLayer("EnemyTypes");
    CGT_ASSERT_ALWAYS(enemyLayer && enemyLayer->getType() == tson::LayerType::ObjectGroup);
    for (auto& enemyData : enemyLayer->getObjectsByType(tson::ObjectType::Object))
    {
        auto& enemyType = enemyTypes.emplace_back();
        enemyType.position = glm::vec2(0.0f);
        enemyType.name = enemyData.getName();
        enemyType.tileId = enemyData.getGid() - rawTileset->getFirstgid();
        enemyType.health = enemyData.get<float>("Health");
        enemyType.armor = enemyData.get<float>("Armor");
        enemyType.speed = enemyData.get<float>("Speed");
        enemyType.unitsPerSpawn = enemyData.get<int>("UnitsPerSpawn");
    }

    std::vector<std::vector<Enemy>> enemies(enemyPaths.size());
    
    std::default_random_engine randEngine;
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    cgt::Clock clock;
    cgt::render::RenderQueue renderQueue;
    cgt::render::RenderStats renderStats {};
    SDL_Event event {};

    bool quitRequested = false;
    while (!quitRequested)
    {
        ZoneScopedN("Main Loop");

        const float dt = clock.Tick();
        imguiHelper->NewFrame(dt, camera);

        {
            ImGui::SetNextWindowSize({200, 80}, ImGuiCond_FirstUseEver);
            ImGui::Begin("Render Stats");
            ImGui::Text("Frame time: %.2fms", dt * 1000.0f);
            ImGui::Text("Sprites: %d", renderStats.spriteCount);
            ImGui::Text("Drawcalls: %d", renderStats.drawcallCount);
            ImGui::End();
        }

        while (window->PollEvent(event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quitRequested = true;
                break;
            case SDL_MOUSEWHEEL:
                auto wheel = event.wheel;
                scaleFactorIdx -= wheel.y;
                scaleFactorIdx = glm::clamp(scaleFactorIdx, 0, (i32)SDL_arraysize(SCALE_FACTORS) - 1);
            }
        }

        glm::vec2 cameraMovInput(0.0f);
        const float CAMERA_SPEED = 5.0f;
        auto* keyboard = SDL_GetKeyboardState(nullptr);
        if (keyboard[SDL_SCANCODE_S])
        {
            cameraMovInput.y -= 1.0f;
        }
        if (keyboard[SDL_SCANCODE_W])
        {
            cameraMovInput.y += 1.0f;
        }
        if (keyboard[SDL_SCANCODE_A])
        {
            cameraMovInput.x -= 1.0f;
        }
        if (keyboard[SDL_SCANCODE_D])
        {
            cameraMovInput.x += 1.0f;
        }

        const float cameraMovInputLengthSqr = glm::dot(cameraMovInput, cameraMovInput);
        if (cameraMovInputLengthSqr > 0.0f)
        {
            cameraMovInput /= glm::sqrt(cameraMovInputLengthSqr);
            camera.position += cameraMovInput * dt * CAMERA_SPEED * (1.0f / SCALE_FACTORS[scaleFactorIdx]);
        }

        camera.pixelsPerUnit = basePPU * SCALE_FACTORS[scaleFactorIdx];

        const float DT_SCALE_FACTORS[] = { 0.0f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
        const char* DT_SCALE_FACTORS_STR[] = { "0", "0.25", "0.5", "1.0", "2.0", "4.0" };
        static u32 selectedDtScaleIdx = 3;
        {
            ImGui::Begin("Gameplay Settings");

            for (u32 i = 0; i < SDL_arraysize(DT_SCALE_FACTORS); ++i)
            {
                if (ImGui::RadioButton(DT_SCALE_FACTORS_STR[i], i == selectedDtScaleIdx))
                {
                    selectedDtScaleIdx = i;
                }
            }

            ImGui::End();
        }

        const float scaledDt = dt * DT_SCALE_FACTORS[selectedDtScaleIdx];

        {
            static u32 selectedEnemyIdx = 0, selectedPathIdx = 0;
            ImGui::Begin("Spawn Enemies");

            if (ImGui::BeginCombo("Enemy Type", enemyTypes[selectedEnemyIdx].name.c_str()))
            {
                for (u32 i = 0; i < enemyTypes.size(); ++i)
                {
                    bool selected = i == selectedEnemyIdx;
                    if (ImGui::Selectable(enemyTypes[i].name.c_str(), selected))
                    {
                        selectedEnemyIdx = i;
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginCombo("Path", enemyPaths[selectedPathIdx].debugName.c_str()))
            {
                for (u32 i = 0; i < enemyPaths.size(); ++i)
                {
                    bool selected = i == selectedPathIdx;
                    if (ImGui::Selectable(enemyPaths[i].debugName.c_str(), selected))
                    {
                        selectedPathIdx = i;
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            static i32 enemiesToSpawn = 20;

            ImGui::InputInt("Enemies To Spawn", &enemiesToSpawn);

            if (ImGui::Button("Spawn Enemy"))
            {
                for (i32 i = 0; i < enemiesToSpawn; ++i)
                {
                    Enemy newEnemy = enemyTypes[selectedEnemyIdx];
                    auto& path = enemyPaths[selectedPathIdx];
                    newEnemy.position = path.waypoints[0];
                    glm::vec2 randomShift(
                        distribution(randEngine),
                        distribution(randEngine));
                    newEnemy.position += randomShift;

                    enemies[selectedPathIdx].emplace_back(newEnemy);
                }
            }

            ImGui::End();
        }

        for (u32 pathIdx = 0; pathIdx < enemyPaths.size(); ++pathIdx)
        {
            auto& path = enemyPaths[pathIdx];
            for (auto& enemy : enemies[pathIdx])
            {
                if (enemy.targetPointIdx >= path.waypoints.size())
                {
                    continue;
                }

                glm::vec2 a = path.waypoints[enemy.targetPointIdx - 1];
                glm::vec2 b = path.waypoints[enemy.targetPointIdx];
                float minX = glm::min(a.x, b.x);
                float maxX = glm::max(a.x, b.x);
                float minY = glm::min(a.y, b.y);
                float maxY = glm::max(a.y, b.y);

                glm::vec2 closestPoint(
                    glm::clamp(enemy.position.x, minX, maxX),
                    glm::clamp(enemy.position.y, minY, maxY));
                float distanceLeft = 2.0f;
                u32 targetPointIdx = enemy.targetPointIdx;
                glm::vec2 chasePoint = closestPoint;

                do
                {
                    glm::vec2 targetPoint = path.waypoints[targetPointIdx];
                    glm::vec2 forward = targetPoint - chasePoint;
                    float advanceDistance = glm::min(distanceLeft, glm::length(forward));
                    chasePoint += glm::normalize(forward) * advanceDistance;
                    distanceLeft -= advanceDistance;
                    ++targetPointIdx;
                } while (distanceLeft > 0.0001f && targetPointIdx < path.waypoints.size());

                glm::vec2 forwardDirection = glm::normalize(b - a);
                glm::vec2 targetPoint = chasePoint;//closestPoint + forwardDirection * 1.0f;

                glm::vec2 target = targetPoint;//path.waypoints[enemy.targetPointIdx];
                glm::vec2 toTarget = target - enemy.position;
                float toTargetDist = glm::length(toTarget);
                float stepLength = enemy.speed * scaledDt;

                glm::vec2 velocity = glm::normalize(toTarget) * enemy.speed;

                for (auto& enemyGroup : enemies)
                {
                    for (auto& otherEnemy : enemyGroup)
                    {
                        if (&enemy == &otherEnemy)
                        {
                            continue;
                        }

                        glm::vec2 fromEnemy = enemy.position - otherEnemy.position;
                        float distanceSqr = glm::dot(fromEnemy, fromEnemy);
                        if (distanceSqr < 0.5f * 0.5f)
                        {
                            velocity += fromEnemy;
                        }
                    }
                }


                if (stepLength >= toTargetDist)
                {
                    ++enemy.targetPointIdx;
                }
                else
                {
                    enemy.position += velocity * scaledDt;
                }
            }
        }

        renderQueue.Reset();
        renderQueue.clearColor = { 0.5f, 0.5f, 0.5f, 1.0f };

        baseMapLayer->Render(renderQueue, *tileset);
        propsMapLayer->Render(renderQueue, *tileset);

        for (u32 pathIdx = 0; pathIdx < enemyPaths.size(); ++pathIdx)
        {
            for (auto& enemy : enemies[pathIdx])
            {
                cgt::render::SpriteDrawRequest sprite;
                sprite.position = enemy.position;
                sprite.texture = tileset->GetTexture();
                auto uv = (*tileset)[enemy.tileId];
                sprite.uvMin = uv.min;
                sprite.uvMax = uv.max;
                renderQueue.sprites.emplace_back(std::move(sprite));
            }
        }

        for (auto& path : enemyPaths)
        {
            path.DebugRender();
        }

        renderStats = render->Submit(renderQueue, camera);
        imguiHelper->RenderUi(camera);
        render->Present();

        TracyPlot("Sprites", (i64)renderStats.spriteCount);
        TracyPlot("Drawcalls", (i64)renderStats.drawcallCount);
    }

    return 0;
}