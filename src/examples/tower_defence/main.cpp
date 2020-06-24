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

        renderQueue.Reset();
        renderQueue.clearColor = { 0.5f, 0.5f, 0.5f, 1.0f };

        baseMapLayer->Render(renderQueue, *tileset);
        propsMapLayer->Render(renderQueue, *tileset);

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