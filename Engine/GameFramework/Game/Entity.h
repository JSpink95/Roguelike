#pragma once
#include "Core.h"
#include "Core\Object.h"
#include "Game\Scene.h"
#include <entt.hpp>

struct Entity
    : public Object
{
    BIND_OBJECT(Entity);

    entt::entity id;
    Scene::Ref world;

    template<typename T, typename...TArgs> T& AddComponent(TArgs&&...args);
    template<typename T> void RemoveComponent();
    template<typename T> T& GetComponent();
    template<typename T> bool HasComponent();
};
#include "Entity.inl"