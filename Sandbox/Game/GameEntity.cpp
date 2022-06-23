#include "Game\GameEntity.h"

#include <Game\Components.h>
#include <Game\physics_components.h>

Entity::Ref CreateEntity(Scene::Ref scene, std::string_view tag/* = ""*/)
{
    auto entity = scene->CreateEntity(tag);
    return entity;
}

Entity::Ref CreateWorldEntity(Scene::Ref scene, std::string_view tag/* = ""*/)
{
    auto entity = CreateEntity(scene, tag);
    entity->AddComponent<TransformComponent>();

    return entity;
}

Entity::Ref CreatePhysicsEntity(Scene::Ref scene, std::string_view tag/* = ""*/)
{
    auto entity = CreateWorldEntity(scene, tag);
    entity->AddComponent<RigidBodyComponent>();

    return entity;
}

Entity::Ref CreateSpriteEntity(Scene::Ref scene, Sprite::Ref sprite, bool withPhysics/* = false*/, std::string_view tag/* = ""*/)
{
    auto entity = withPhysics ? CreatePhysicsEntity(scene, tag) : CreateWorldEntity(scene, tag);
    entity->AddComponent<SpriteComponent>().sprite = sprite;

    return entity;
}