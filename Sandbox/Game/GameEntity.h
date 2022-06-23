#pragma once
#include <Core.h>
#include <Game\Entity.h>
#include <Game\Scene.h>
#include <Game\Sprite.h>

Entity::Ref CreateEntity(Scene::Ref scene, std::string_view tag = "");
Entity::Ref CreateWorldEntity(Scene::Ref scene, std::string_view tag = "");
Entity::Ref CreatePhysicsEntity(Scene::Ref scene, std::string_view tag = "");
Entity::Ref CreateSpriteEntity(Scene::Ref scene, Sprite::Ref sprite, bool withPhysics = false, std::string_view tag = "");
