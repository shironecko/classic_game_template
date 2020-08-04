#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/entity_types.h>
#include <examples/tower_defence/entities.h>
#include <examples/tower_defence/map_data.h>
#include <examples/tower_defence/game_state.h>
#include <examples/tower_defence/game_session.h>
#include <examples/tower_defence/helper_functions.h>

int GameMain()
{
    auto window = cgt::WindowConfig::Default()
        .WithTitle("Tower Defence")
        .WithDimensions(1920, 1080)
        .Build();

    auto render = cgt::render::RenderConfig::Default(window)
        .Build();

    auto imguiHelper = cgt::ImGuiHelper::Create(window, render);

    cgt::EventLoop eventLoop(window);
    eventLoop.AddEventListener(imguiHelper);

    const float basePPU = 64.0f;
    const float SCALE_FACTORS[] = { 4.0f, 3.0f, 2.0f, 1.0f, 1.0f / 2.0f, 1.0f / 3.0f, 1.0f / 4.0f, 1.0f / 5.0f, 1.0f / 6.0f, 1.0f / 7.0f, 1.0f / 8.0f, 1.0f / 9.0f, 1.0f / 10.0f };
    i32 scaleFactorIdx = 3;

    cgt::render::CameraSimpleOrtho camera(*window);
    camera.pixelsPerUnit = 64.0f;

    GameCommandQueue gameCommands;
    GameEventQueue gameEvents;

    // https://www.gafferongames.com/post/fix_your_timestep
    const float FIXED_DELTA = 1.0f / 30.0f;
    GameState interpolatedState;

    cgt::Clock clock;
    float accumulatedDelta = 0.0f;
    cgt::render::RenderStats renderStats;
    SDL_Event event {};

    const float DT_SCALE_FACTORS[] = { 0.0f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
    const char* DT_SCALE_FACTORS_STR[] = { "0", "0.25", "0.5", "1.0", "2.0", "4.0" };
    u32 selectedDtScaleIdx = 3;

    u32 selectedTowerTypeId = 0;

    auto gameSession = GameSession::FromMap(cgt::AssetPath("examples/maps/tower_defense.json"), *render, FIXED_DELTA);
    cgt::render::SpriteDrawList effectsDrawList;

    bool quitRequested = false;
    while (!quitRequested)
    {
        ZoneScopedN("Main Loop");

        effectsDrawList.clear();

        const float dt = clock.Tick();
        {
            const float scaledDt = dt * DT_SCALE_FACTORS[selectedDtScaleIdx];
            accumulatedDelta += scaledDt;
        }

        bool lmbWasClicked = false;
        while (eventLoop.PollEvent(event))
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

        imguiHelper->NewFrame(dt, camera);

        // NOTE: prone to "spiral of death"
        // see https://www.gafferongames.com/post/fix_your_timestep/
        while (accumulatedDelta > FIXED_DELTA)
        {
            accumulatedDelta -= FIXED_DELTA;
            gameSession->TimeStep(gameCommands, gameEvents);
            gameCommands.clear();

            // FIXME: actually make use of events
            gameEvents.clear();
        }

        const float interpolationFactor = glm::smoothstep(0.0f, FIXED_DELTA, accumulatedDelta);
        gameSession->InterpolateState(interpolatedState, interpolationFactor);

        {
            ImGui::SetNextWindowSize({200, 80}, ImGuiCond_FirstUseEver);
            ImGui::Begin("Render Stats");
            ImGui::Text("Frame time: %.2fms", dt * 1000.0f);
            ImGui::Text("Sprites: %u", renderStats.spriteCount);
            ImGui::Text("Drawcalls: %u", renderStats.drawcallCount);
            ImGui::End();
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
        auto tilePos = gameSession->mapData.buildableMap.WorldToTile(world);
        bool buildable = gameSession->mapData.buildableMap.Query(world);

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


            const TowerType& towerType = gameSession->mapData.towerTypes[selectedTowerTypeId];

            Im3d::DrawCircle(glm::vec3(tilePos, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), towerType.range);

            auto& sprite = effectsDrawList.AddSprite();
            gameSession->tilesetHelper->GetTileSpriteSrc(towerType.tileId, sprite.src);
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

            if (ImGui::BeginCombo("Enemy Type", gameSession->mapData.enemyTypes[selectedEnemyIdx].name.c_str()))
            {
                for (u32 i = 0; i < gameSession->mapData.enemyTypes.size(); ++i)
                {
                    bool selected = i == selectedEnemyIdx;
                    if (ImGui::Selectable(gameSession->mapData.enemyTypes[i].name.c_str(), selected))
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

            if (ImGui::Button("Add 100 Gold"))
            {
                auto& gameCmd = gameCommands.emplace_back();
                gameCmd.type = GameCommand::Type::Debug_AddGold;
                auto& cmdData = gameCmd.data.debug_addGoldData;
                cmdData.amount = 100.0f;
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Towers");
            for (u32 i = 0; i <gameSession->mapData.towerTypes.size(); ++i)
            {
                TowerType& towerType = gameSession->mapData.towerTypes[i];
                ProjectileType& projectileType = gameSession->mapData.projectileTypes[towerType.projectileTypeIdx];
                cgt::render::SpriteSource spriteSrc;
                gameSession->tilesetHelper->GetTileSpriteSrc(towerType.tileId, spriteSrc);
                auto textureId = render->GetImTextureID(spriteSrc.texture);

                ImGui::ImageButton(textureId, { 64.0f, 64.0f }, { spriteSrc.uv.min.x, spriteSrc.uv.min.y }, { spriteSrc.uv.max.x, spriteSrc.uv.max.y });

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
                    ImGui::TextColored({ 0.8f, 0.2f, 0.2f, 1.0f }, "%.0f", projectileType.damage);

                    if (projectileType.splashRadius > 0.0f)
                    {
                        ImGui::Text("Splash: ");
                        ImGui::SameLine();
                        ImGui::TextColored({ 0.8f, 0.2f, 0.2f, 1.0f }, "%.1f", projectileType.splashRadius);
                    }

                    ImGui::Text("Shots per second: ");
                    ImGui::SameLine();
                    ImGui::TextColored({ 0.2f, 0.8f, 0.2f, 1.0f }, "%.1f", towerType.shotsPerSecond);

                    ImGui::EndTooltip();
                }

                if (ImGui::IsItemClicked())
                {
                    selectedTowerTypeId = i;
                }

                ImGui::SameLine();
            }
            ImGui::End();
        }

        imguiHelper->BeginInvisibleFullscreenWindow();
        interpolatedState.ForEachEnemy(gameSession->mapData, [&](auto& enemy, auto& type) {
            auto& enemyType = gameSession->mapData.enemyTypes[enemy.typeIdx];

            if (cgt::math::AreNearlyEqUlps(enemy.remainingHealth, enemyType.maxHealth)
                || cgt::math::IsNearlyZero(enemy.remainingHealth))
            {
                return;
            }

            glm::vec2 hpOffset(-0.5f, 0.5f);
            glm::vec2 hpDim(1.0f, 0.2f);

            glm::vec2 screenPosition = camera.WorldToScreen(enemy.position + hpOffset);

            ImGui::SetCursorPos({ screenPosition.x, screenPosition.y });
            glm::vec2 screenSize = cgt::math::WorldToPixels(hpDim, camera.pixelsPerUnit);
            ImGui::ProgressBar(enemy.remainingHealth / enemyType.maxHealth, { screenSize.x, screenSize.y } , "");

        });
        imguiHelper->EndInvisibleFullscreenWindow();

        gameSession->mapData.enemyPath.DebugRender();

        renderStats.Reset();
        render->Clear({ 0.2f, 0.2f, 0.2f, 1.0f });
        renderStats += gameSession->RenderWorld(interpolatedState, *render, camera);
        renderStats += render->Submit(effectsDrawList, camera, false);
        imguiHelper->RenderUi(camera);
        render->Present();

        TracyPlot("Sprites", (i64)renderStats.spriteCount);
        TracyPlot("Drawcalls", (i64)renderStats.drawcallCount);
    }

    return 0;
}