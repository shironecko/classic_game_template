#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/map_data.h>

void EnemyPath::Load(tson::Map& map, EnemyPath& outPath)
{
    outPath.waypoints.clear();

    auto* pathLayer = map.getLayer("Paths");
    CGT_ASSERT_ALWAYS(pathLayer && pathLayer->getType() == tson::LayerType::ObjectGroup);

    auto paths = pathLayer->getObjectsByType(tson::ObjectType::Polyline);
    CGT_ASSERT_ALWAYS(!paths.empty());
    auto& object = paths[0];
    outPath.debugName = object.getName();
    auto color = object.get<tson::Colori>("Color").asFloat();
    outPath.debugColor = { color.r, color.g, color.b, color.a };

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

        outPath.waypoints.emplace_back(finalPosition);
    }

    outPath.distancesToGoal.resize(outPath.waypoints.size(), 0.0f);
    for (i32 i = outPath.waypoints.size() - 2; i >= 0; --i)
    {
        const glm::vec2 a = outPath.waypoints[i];
        const glm::vec2 b = outPath.waypoints[i + 1];
        outPath.distancesToGoal[i] = outPath.distancesToGoal[i + 1] + glm::distance(a, b);
    }
}

void EnemyPath::DebugRender()
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

void BuildableMap::Load(tson::Map& map, BuildableMap& outMap)
{
    auto size = map.getSize();
    outMap.m_Width = size.x;
    outMap.m_Height = size.y;
    outMap.m_Grid.clear();
    outMap.m_Grid.resize(size.x * size.y, 0);

    auto* baseLayer = map.getLayer("Base");
    CGT_ASSERT_ALWAYS(baseLayer && baseLayer->getType() == tson::LayerType::TileLayer);
    for (auto &[pos, tile] : baseLayer->getTileData())
    {
        bool buildable = tile->get<bool>("BuildingAllowed");

        u32 x = std::get<0>(pos);
        u32 y = std::get<1>(pos);
        outMap.At(x, y) = buildable ? 1 : 0;
    }
}

void MapData::Load(tson::Map& map, MapData& outMapData)
{
    EnemyPath::Load(map, outMapData.enemyPath);
    LoadEntityTypes(map, outMapData.enemyTypes, outMapData.towerTypes, outMapData.projectileTypes);
    BuildableMap::Load(map, outMapData.buildableMap);
}

