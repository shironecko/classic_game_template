#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/game.h>
#include <examples/tower_defence/helper_functions.h>

void Game::Initialize(cgt::Engine& engine)
{
    m_Camera.windowDimensions = engine.GetWindowDimensions();
    m_Camera.pixelsPerUnit = 64.0f;

    m_GameSession = GameSession::FromMap(cgt::AssetPath("examples/maps/tower_defense.json"), engine, FIXED_DELTA);
}

cgt::IGame::ControlFlow Game::Update(cgt::Engine& engine, float deltaTime, bool quitRequestedByUser)
{
    m_Camera.windowDimensions = engine.GetWindowDimensions();

    const float basePPU = 64.0f;
    const float SCALE_FACTORS[] = { 4.0f, 3.0f, 2.0f, 1.0f, 1.0f / 2.0f, 1.0f / 3.0f, 1.0f / 4.0f, 1.0f / 5.0f, 1.0f / 6.0f, 1.0f / 7.0f, 1.0f / 8.0f, 1.0f / 9.0f, 1.0f / 10.0f };
    static i32 scaleFactorIdx = 3;

    static float accumulatedDelta = 0.0f;
    const float DT_SCALE_FACTORS[] = { 0.0f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
    const char* DT_SCALE_FACTORS_STR[] = { "0", "0.25", "0.5", "1.0", "2.0", "4.0" };
    static u32 selectedDtScaleIdx = 3;

    static u32 selectedTowerTypeId = 0;

    m_EffectsDrawList.clear();

    {
        const float scaledDt = deltaTime * DT_SCALE_FACTORS[selectedDtScaleIdx];
        accumulatedDelta += scaledDt;
    }

    int mouseWheelMotion = 0;
    if (engine.GetInput().IsKeyPressed(cgt::KeyCode::MouseWheelUp))
    {
        mouseWheelMotion = 1;
    }
    else if (engine.GetInput().IsKeyPressed(cgt::KeyCode::MouseWheelDown))
    {
        mouseWheelMotion = -1;
    }
    scaleFactorIdx -= mouseWheelMotion;
    scaleFactorIdx = glm::clamp(scaleFactorIdx, 0, (i32)CGT_ARRAY_LENGTH(SCALE_FACTORS) - 1);


    // NOTE: prone to "spiral of death"
    // see https://www.gafferongames.com/post/fix_your_timestep/
    while (accumulatedDelta > FIXED_DELTA)
    {
        accumulatedDelta -= FIXED_DELTA;
        m_GameSession->TimeStep(m_GameCommands, m_GameEvents);
        m_GameCommands.clear();

        // FIXME: actually make use of events
        m_GameEvents.clear();
    }

    const float interpolationFactor = glm::smoothstep(0.0f, FIXED_DELTA, accumulatedDelta);
    m_GameSession->InterpolateState(m_InterpolatedState, interpolationFactor);

    glm::vec2 cameraMovInput(0.0f);
    const float CAMERA_SPEED = 5.0f;
    if (engine.GetInput().IsKeyHeld(cgt::KeyCode::S))
    {
        cameraMovInput.y -= 1.0f;
    }
    if (engine.GetInput().IsKeyHeld(cgt::KeyCode::W))
    {
        cameraMovInput.y += 1.0f;
    }
    if (engine.GetInput().IsKeyHeld(cgt::KeyCode::A))
    {
        cameraMovInput.x -= 1.0f;
    }
    if (engine.GetInput().IsKeyHeld(cgt::KeyCode::D))
    {
        cameraMovInput.x += 1.0f;
    }

    const float cameraMovInputLengthSqr = glm::dot(cameraMovInput, cameraMovInput);
    if (cameraMovInputLengthSqr > 0.0f)
    {
        cameraMovInput /= glm::sqrt(cameraMovInputLengthSqr);
        m_Camera.position += cameraMovInput * deltaTime * CAMERA_SPEED * (1.0f / SCALE_FACTORS[scaleFactorIdx]);
    }

    m_Camera.pixelsPerUnit = basePPU * SCALE_FACTORS[scaleFactorIdx];


    auto mouse = engine.GetInput().GetMousePosition();
    glm::vec2 world = m_Camera.ScreenToWorld(mouse.x, mouse.y);
    auto tilePos = m_GameSession->mapData.buildableMap.WorldToTile(world);
    bool buildable = m_GameSession->mapData.buildableMap.Query(world);

    {
        ImGui::BeginMainMenuBar();

        ImGui::TextUnformatted("Gold ");
        ImGui::TextColored({ 0.9f, 0.8f, 0.2f, 1.0f }, "%.0f$", m_InterpolatedState.playerState.gold);

        ImGui::TextUnformatted("Lives ");
        ImGui::TextColored({ 0.8f, 0.2f, 0.2f, 1.0f }, "%u", m_InterpolatedState.playerState.lives);

        ImGui::EndMainMenuBar();
    }

    {
        ImGui::Begin("Mouse To World");

        ImGui::Text("Screen position: (%d, %d)", mouse.x, mouse.y);
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


        const TowerType& towerType = m_GameSession->mapData.towerTypes[selectedTowerTypeId];

        Im3d::DrawCircle(glm::vec3(tilePos, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), towerType.range);

        auto& sprite = m_EffectsDrawList.AddSprite();
        m_GameSession->tilesetHelper->GetTileSpriteSrc(towerType.tileId, sprite.src);
        sprite.position = glm::vec2(tilePos.x, tilePos.y);
        sprite.colorTint = glm::vec4(glm::vec3(selectionColor), 0.3f);
        sprite.layer = 5;
    }

    if (engine.GetInput().IsKeyPressed(cgt::KeyCode::LeftMouseButton) && buildable)
    {
        auto& gameCmd = m_GameCommands.emplace_back();
        gameCmd.type = GameCommand::Type::BuildTower;
        auto& cmdData = gameCmd.data.buildTowerData;
        cmdData.towerType = selectedTowerTypeId;
        cmdData.position = glm::vec2(tilePos.x, tilePos.y);
    }

    {
        ImGui::Begin("Gameplay Settings");

        for (u32 i = 0; i < CGT_ARRAY_LENGTH(DT_SCALE_FACTORS); ++i)
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

        if (ImGui::BeginCombo("Enemy Type", m_GameSession->mapData.enemyTypes[selectedEnemyIdx].name.c_str()))
        {
            for (u32 i = 0; i < m_GameSession->mapData.enemyTypes.size(); ++i)
            {
                bool selected = i == selectedEnemyIdx;
                if (ImGui::Selectable(m_GameSession->mapData.enemyTypes[i].name.c_str(), selected))
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
                auto& gameCmd = m_GameCommands.emplace_back();
                gameCmd.type = GameCommand::Type::Debug_SpawnEnemy;
                auto& cmdData = gameCmd.data.debug_spawnEnemyData;
                cmdData.enemyType = selectedEnemyIdx;
            }
        }

        if (ImGui::Button("Despawn All"))
        {
            auto& gameCmd = m_GameCommands.emplace_back();
            gameCmd.type = GameCommand::Type::Debug_DespawnAllEnemies;
        }

        if (ImGui::Button("Add 100 Gold"))
        {
            auto& gameCmd = m_GameCommands.emplace_back();
            gameCmd.type = GameCommand::Type::Debug_AddGold;
            auto& cmdData = gameCmd.data.debug_addGoldData;
            cmdData.amount = 100.0f;
        }

        ImGui::End();
    }

    {
        ImGui::Begin("Towers");
        for (u32 i = 0; i < m_GameSession->mapData.towerTypes.size(); ++i)
        {
            TowerType& towerType = m_GameSession->mapData.towerTypes[i];
            ProjectileType& projectileType = m_GameSession->mapData.projectileTypes[towerType.projectileTypeIdx];
            cgt::render::SpriteSource spriteSrc;
            m_GameSession->tilesetHelper->GetTileSpriteSrc(towerType.tileId, spriteSrc);
            auto textureId = engine.GetImGuiTextureID(spriteSrc.texture);

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

    cgt::ImGuiHelper::BeginInvisibleFullscreenWindow();
    m_InterpolatedState.ForEachEnemy(m_GameSession->mapData, [&](auto& enemy, auto& type) {
        auto& enemyType = m_GameSession->mapData.enemyTypes[enemy.typeIdx];

        if (cgt::math::AreNearlyEqUlps(enemy.remainingHealth, enemyType.maxHealth)
            || cgt::math::IsNearlyZero(enemy.remainingHealth))
        {
            return;
        }

        glm::vec2 hpOffset(-0.5f, 0.5f);
        glm::vec2 hpDim(1.0f, 0.2f);

        glm::vec2 screenPosition = m_Camera.WorldToScreen(enemy.position + hpOffset);

        ImGui::SetCursorPos({ screenPosition.x, screenPosition.y });
        glm::vec2 screenSize = cgt::math::WorldToPixels(hpDim, m_Camera.pixelsPerUnit);
        ImGui::ProgressBar(enemy.remainingHealth / enemyType.maxHealth, { screenSize.x, screenSize.y } , "");

    });
    cgt::ImGuiHelper::EndInvisibleFullscreenWindow();

    m_GameSession->mapData.enemyPath.DebugRender();

    m_GameSession->RenderWorld(m_InterpolatedState, engine);
    engine.RenderSprites(m_EffectsDrawList, false);

    return quitRequestedByUser
        ? cgt::IGame::ControlFlow::TerminateGame
        : cgt::IGame::ControlFlow::ContinueRunning;
}

void Game::Shutdown(cgt::Engine& engine)
{

}
