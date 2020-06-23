#include <examples/tower_defence/pch.h>

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

    tson::Tileset* tileset = map.getTileset("tower_defense");
    CGT_ASSERT_ALWAYS(tileset);

    cgt::render::TextureHandle tilesetTexture = render->LoadTexture(
        cgt::AssetPath("examples/maps") / tileset->getImagePath());

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

        int firstId = tileset->getFirstgid();
        int columns = tileset->getColumns();
        int rows = tileset->getTileCount() / columns;
        int lastId = (tileset->getFirstgid() + tileset->getTileCount()) - 1;

        for (auto& layer: map.getLayers())
        {
            ZoneScopedN("Layer Render");

            if (layer.getType() != tson::LayerType::TileLayer)
            {
                continue;
            }

            for (auto& [pos, tile]: layer.getTileData())
            {
                //Get position in pixel units
                tson::Vector2i position = {std::get<0>(pos) * map.getTileSize().x,std::get<1>(pos) * map.getTileSize().y};
                
                int baseTilePosition = (tile->getId() - firstId); //This will determine the base position of the tile.
                
                //The baseTilePosition can be used to calculate offset on its related tileset image.
                int tileModX = (baseTilePosition % columns);
                int currentRow = (baseTilePosition / columns);
                int offsetX = (tileModX != 0) ? ((tileModX) * map.getTileSize().x) : (0 * map.getTileSize().x);
                int offsetY =  (currentRow < rows-1) ? (currentRow * map.getTileSize().y) : 
                               ((rows-1) * map.getTileSize().y);

                glm::vec2 uvMin = { (float)offsetX / tileset->getImageSize().x, (float)offsetY / tileset->getImageSize().y };
                glm::vec2 uvTileDimensions = { (float)map.getTileSize().x / tileset->getImageSize().x, (float)map.getTileSize().y / tileset->getImageSize().y };
                glm::vec2 uvMax = uvMin + uvTileDimensions;

                cgt::render::SpriteDrawRequest sprite;
                sprite.position = { std::get<0>(pos), std::get<1>(pos) * -1.0f };
                sprite.texture = tilesetTexture;
                sprite.uvMin = uvMin;
                sprite.uvMax = uvMax;

                renderQueue.sprites.emplace_back(std::move(sprite));
            }
        }

        for (auto& layer: map.getLayers())
        {
            if (layer.getType() != tson::LayerType::ObjectGroup)
            {
                continue;
            }

            glm::vec3 readabilityShift(0.0f);
            for (auto& object: layer.getObjectsByType(tson::ObjectType::Polyline))
            {
                auto color = object.get<tson::Colori>("PlayerColor").asFloat();
                glm::vec3 basePosition(
                    (float)object.getPosition().x / map.getTileSize().x - 0.5f,
                    (float)object.getPosition().y / map.getTileSize().y * -1.0f + 0.5f,
                    0.0f);

                glm::mat4 baseRotation = glm::rotate(
                    glm::mat4(1.0f),
                    glm::radians(object.getRotation()),
                    { 0.0f, 0.0f, 1.0f });

                Im3d::PushColor({ color.r, color.g, color.b, color.a });
                Im3d::PushMatrix(glm::translate(glm::mat4(1.0f), readabilityShift));
                Im3d::PushSize(3.0f);
                Im3d::BeginLineStrip();
                bool namePrinted = false;
                for (auto& point: object.getPolylines())
                {
                    glm::vec3 pointPosition(
                        (float)point.x / map.getTileSize().x,
                        (float)point.y / map.getTileSize().y * -1.0f,
                        0.0f);

                    glm::vec3 finalPosition = baseRotation * glm::vec4(pointPosition, 1.0f);
                    finalPosition += basePosition;
                    Im3d::Vertex(finalPosition);

                    if (!namePrinted)
                    {
                        namePrinted = true;
                        Im3d::Text(finalPosition, Im3d::TextFlags_AlignTop, object.getName().c_str());
                    }
                }
                Im3d::End();
                Im3d::PopSize();
                Im3d::PopMatrix();
                Im3d::PopColor();

                readabilityShift += glm::vec3(0.1f, -0.1f, 0.0f);
            }
        }

        renderStats = render->Submit(renderQueue, camera);
        imguiHelper->RenderUi(camera);
        render->Present();

        TracyPlot("Sprites", (i64)renderStats.spriteCount);
        TracyPlot("Drawcalls", (i64)renderStats.drawcallCount);
    }

    return 0;
}