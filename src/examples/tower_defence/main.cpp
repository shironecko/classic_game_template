#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/enemy_data.h>
#include <examples/tower_defence/tower_data.h>
#include <examples/tower_defence/map_data.h>
#include <examples/tower_defence/game_state.h>

int GameMain()
{
    auto window = cgt::WindowConfig::Default()
        .WithTitle("Tower Defence")
        .WithDimensions(1920, 1080)
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

    auto tilesetHelper = cgt::TilesetHelper::LoadMapTilesets(map, cgt::AssetPath("examples/maps"), *render);
    cgt::render::SpriteDrawList staticMapDrawList;
    tilesetHelper->RenderTileLayers(map, staticMapDrawList, 0);

    MapData mapData;
    MapData::Load(map, mapData);

    GameCommandQueue gameCommands;

    // https://www.gafferongames.com/post/fix_your_timestep
    const float FIXED_DELTA = 1.0f / 30.0f;
    GameState gameStates[2];
    GameState* prevState = &gameStates[0];
    GameState* nextState = &gameStates[1];

    prevState->playerState.gold = 100;
    prevState->playerState.lives = 20;
    // warm-up
    GameState::TimeStep(mapData, *prevState, *nextState, gameCommands, FIXED_DELTA);

    GameState interpolatedState;


    cgt::Clock clock;
    float accumulatedDelta = 0.0f;
    cgt::render::SpriteDrawList drawList;
    cgt::render::RenderStats renderStats;
    SDL_Event event {};

    const float DT_SCALE_FACTORS[] = { 0.0f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
    const char* DT_SCALE_FACTORS_STR[] = { "0", "0.25", "0.5", "1.0", "2.0", "4.0" };
    u32 selectedDtScaleIdx = 3;

    u32 selectedTowerTypeId = 0;

    bool quitRequested = false;
    while (!quitRequested)
    {
        ZoneScopedN("Main Loop");

        drawList.clear();

        const float dt = clock.Tick();
        {
            const float scaledDt = dt * DT_SCALE_FACTORS[selectedDtScaleIdx];
            accumulatedDelta += scaledDt;
        }

        imguiHelper->NewFrame(dt, camera);

        // NOTE: prone to "spiral of death"
        // see https://www.gafferongames.com/post/fix_your_timestep/
        while (accumulatedDelta > FIXED_DELTA)
        {
            accumulatedDelta -= FIXED_DELTA;
            std::swap(prevState, nextState);
            GameState::TimeStep(mapData, *prevState, *nextState, gameCommands, FIXED_DELTA);
            gameCommands.clear();
        }

        const float interpolationFactor = glm::smoothstep(0.0f, FIXED_DELTA, accumulatedDelta);
        GameState::Interpolate(*prevState, *nextState, interpolatedState, interpolationFactor);

        {
            ImGui::SetNextWindowSize({200, 80}, ImGuiCond_FirstUseEver);
            ImGui::Begin("Render Stats");
            ImGui::Text("Frame time: %.2fms", dt * 1000.0f);
            ImGui::Text("Sprites: %u", renderStats.spriteCount);
            ImGui::Text("Drawcalls: %u", renderStats.drawcallCount);
            ImGui::End();
        }

        bool lmbWasClicked = false;
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
                break;
            case SDL_MOUSEBUTTONDOWN:
                auto button = event.button;
                lmbWasClicked = button.button == SDL_BUTTON_LEFT;
                break;
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


        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        glm::vec2 world = camera.ScreenToWorld((u32)mouseX, (u32)mouseY);
        auto tilePos = mapData.buildableMap.WorldToTile(world);
        bool buildable = mapData.buildableMap.Query(world);

        {
            ImGui::BeginMainMenuBar();

            ImGui::TextUnformatted("Gold ");
            ImGui::TextColored({ 0.9f, 0.8f, 0.2f, 1.0f }, "%.0f$", interpolatedState.playerState.gold);

            ImGui::TextUnformatted("Lives ");
            ImGui::TextColored({ 0.8f, 0.2f, 0.2f, 1.0f }, "%u", interpolatedState.playerState.lives);

            ImGui::EndMainMenuBar();
        }

        {
            ImGui::Begin("Mouse To World");

            ImGui::Text("Screen position: (%d, %d)", mouseX, mouseY);
            ImGui::Text("World position: (%.2f, %.2f)", world.x, world.y);
            ImGui::Text("Tile Pos: (%d, %d)", tilePos.x, tilePos.y);
            ImGui::Text("Buildable: %d", (int)buildable);

            ImGui::End();
        }

        auto selectionColor = buildable
            ? glm::vec4(0.0f, 1.0f, 0.0f, 0.1f)
            : glm::vec4(1.0f, 0.0f, 0.0f, 0.1f);
        {
            Im3d::PushColor(Im3d::Color(selectionColor));
            Im3d::DrawAlignedBoxFilled({ tilePos.x - 0.5f, tilePos.y - 0.5f, 0.0f }, { tilePos.x + 0.5f, tilePos.y + 0.5f, 0.0f });
            Im3d::PopColor();

            const TowerType& towerType = mapData.towerTypes[selectedTowerTypeId];

            auto& sprite = drawList.AddSprite();
            tilesetHelper->GetTileSpriteSrc(towerType.tileId, sprite.src);
            sprite.position = glm::vec2(tilePos.x, tilePos.y);
            sprite.colorTint = glm::vec4(glm::vec3(selectionColor), 0.3f);
            sprite.layer = 5;
        }

        if (lmbWasClicked && buildable)
        {
            auto& gameCmd = gameCommands.emplace_back();
            gameCmd.type = GameCommand::Type::BuildTower;
            auto& cmdData = gameCmd.data.buildTowerData;
            cmdData.towerType = selectedTowerTypeId;
            cmdData.position = glm::vec2(tilePos.x, tilePos.y);
        }

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

        {
            static u32 selectedEnemyIdx = 0;
            ImGui::Begin("Spawn Enemies");

            if (ImGui::BeginCombo("Enemy Type", mapData.enemyTypes[selectedEnemyIdx].name.c_str()))
            {
                for (u32 i = 0; i < mapData.enemyTypes.size(); ++i)
                {
                    bool selected = i == selectedEnemyIdx;
                    if (ImGui::Selectable(mapData.enemyTypes[i].name.c_str(), selected))
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

            static i32 enemiesToSpawn = 20;

            ImGui::InputInt("Enemies To Spawn", &enemiesToSpawn);

            if (ImGui::Button("Spawn Enemy"))
            {
                for (i32 i = 0; i < enemiesToSpawn; ++i)
                {
                    auto& gameCmd = gameCommands.emplace_back();
                    gameCmd.type = GameCommand::Type::Debug_SpawnEnemy;
                    auto& cmdData = gameCmd.data.debug_spawnEnemyData;
                    cmdData.enemyType = selectedEnemyIdx;
                }
            }

            if (ImGui::Button("Despawn All"))
            {
                auto& gameCmd = gameCommands.emplace_back();
                gameCmd.type = GameCommand::Type::Debug_DespawnAllEnemies;
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Towers");
            for (u32 i = 0; i <mapData.towerTypes.size(); ++i)
            {
                TowerType& towerType = mapData.towerTypes[i];
                cgt::render::SpriteSource spriteSrc;
                tilesetHelper->GetTileSpriteSrc(towerType.tileId, spriteSrc);
                auto textureId = render->GetImTextureID(spriteSrc.texture);

                if (ImGui::ImageButton(textureId, { 64.0f, 64.0f }, { spriteSrc.uv.min.x, spriteSrc.uv.min.y }, { spriteSrc.uv.max.x, spriteSrc.uv.max.y }))
                {
                    selectedTowerTypeId = i;
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();

                    ImGui::Text("%s", towerType.name.c_str());
                    ImGui::Separator();

                    ImGui::TextColored({ 0.9f, 0.8f, 0.2f, 1.0f }, "%.0f$", towerType.cost);

                    ImGui::Text("Range: ");
                    ImGui::SameLine();
                    ImGui::TextColored({ 0.2f, 0.8f, 0.2f, 1.0f }, "%.0f tiles", towerType.range);

                    ImGui::Text("Damage: ");
                    ImGui::SameLine();
                    ImGui::TextColored({ 0.8f, 0.2f, 0.2f, 1.0f }, "%.0f", towerType.damage);

                    if (towerType.splashRadius > 0.0f)
                    {
                        ImGui::Text("Splash: ");
                        ImGui::SameLine();
                        ImGui::TextColored({ 0.8f, 0.2f, 0.2f, 1.0f }, "%.1f", towerType.splashRadius);
                    }

                    ImGui::Text("Shots per second: ");
                    ImGui::SameLine();
                    ImGui::TextColored({ 0.2f, 0.8f, 0.2f, 1.0f }, "%.1f", towerType.shotsPerSecond);

                    ImGui::EndTooltip();
                }

                ImGui::SameLine();
            }
            ImGui::End();
        }

        for (auto& enemy : interpolatedState.enemies)
        {
            auto& enemyType = mapData.enemyTypes[enemy.type];

            auto& sprite = drawList.AddSprite();
            sprite.rotation = cgt::math::VectorAngle(enemy.direction);
            sprite.position = enemy.position;
            tilesetHelper->GetTileSpriteSrc(enemyType.tileId, sprite.src);

            if (cgt::math::AreNearlyEqUlps(enemy.remainingHealth, enemyType.maxHealth))
            {
                continue;
            }

            glm::vec2 hpOffset(-0.5f, 0.5f);
            glm::vec2 hpDim(1.0f, 0.3f);
            glm::vec2 hpMargin(0.01f, 0.01f);
            glm::vec4 hpBgColor(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec4 hpBarColor(0.2f, 0.8f, 0.2f, 1.0f);

//            Im3d::PushColor(Im3d::Color(hpBgColor));
//            Im3d::DrawAlignedBoxFilled(
//                glm::vec3(enemy.position - hpDim * 0.5f, 0.0f),
//                glm::vec3(enemy.position + hpDim * 0.5f, 0.0f));
//            Im3d::PopColor();
//
//            Im3d::PushColor(Im3d::Color(hpBarColor));
//            Im3d::DrawAlignedBoxFilled(
//                glm::vec3(enemy.position - hpDim * 0.5f + hpMargin, -0.1f),
//                glm::vec3(enemy.position + hpDim * 0.5f - hpMargin, -0.1f));
//            Im3d::PopColor();

            glm::vec2 screenPosition = camera.WorldToScreen(enemy.position + hpOffset);

            imguiHelper->BeginInvisibleFullscreenWindow();

            ImGui::SetCursorPos({ screenPosition.x, screenPosition.y });
            glm::vec2 screenSize = cgt::math::WorldToPixels(glm::vec2(1.0f, 0.2f), camera.pixelsPerUnit);
            ImGui::ProgressBar(enemy.remainingHealth / enemyType.maxHealth, { screenSize.x, screenSize.y } , "");

            imguiHelper->EndInvisibleFullscreenWindow();

            //ImGui::ProgressBar()
        }

        for (auto& tower : interpolatedState.towers)
        {
            auto& towerType = mapData.towerTypes[tower.type];

            auto& sprite = drawList.AddSprite();
            sprite.position = tower.position;
            tilesetHelper->GetTileSpriteSrc(towerType.tileId, sprite.src);
            sprite.layer = 3;
        }

        for (auto& proj : interpolatedState.projectiles)
        {
            auto& parentTowerType = mapData.towerTypes[proj.parentTowerTypeId];

            auto& sprite = drawList.AddSprite();
            sprite.position = proj.position;
            tilesetHelper->GetTileSpriteSrc(parentTowerType.projectileTileId, sprite.src);
            sprite.layer = 5;
        }

        mapData.enemyPath.DebugRender();

        renderStats.Reset();
        render->Clear({ 0.2f, 0.2f, 0.2f, 1.0f });
        renderStats += render->Submit(staticMapDrawList, camera, false);
        renderStats += render->Submit(drawList, camera);
        imguiHelper->RenderUi(camera);
        render->Present();

        TracyPlot("Sprites", (i64)renderStats.spriteCount);
        TracyPlot("Drawcalls", (i64)renderStats.drawcallCount);
    }

    return 0;
}