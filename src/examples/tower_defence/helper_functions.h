#pragma once

#include <examples/tower_defence/entity_types.h>
#include <examples/tower_defence/entities.h>

inline void RenderEntity(const Entity& entity, const EntityType& type, const cgt::TilesetHelper& tileset, cgt::render::SpriteDrawList& drawList)
{
    auto& sprite = drawList.AddSprite();
    sprite.position = entity.position;
    sprite.rotation = entity.rotation;
    tileset.GetTileSpriteSrc(type.tileId, sprite.src);
}

