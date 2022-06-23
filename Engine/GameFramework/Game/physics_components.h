#pragma once
#include "GameCore.h"
#include "Game\components.h"

enum class RigidBodyType: u8
{
    STATIC,
    DYNAMIC,
    KINEMATIC
};

struct RigidBodyComponent : public IComponent
{
    BIND_OBJECT(RigidBodyComponent);

    RigidBodyType type = RigidBodyType::STATIC;
    bool fixedRotation = false;
    float linearDamping = 1.0f;
    float angularDamping = 0.0f;

    void* runtimeBody = nullptr;
};

struct PhysicsMaterial
{
    bool isTrigger = false;
    float density = 1.0f;
    float friction = 0.5f;
    float restitution = 0.0f;
    float restitutionThreshold = 0.5f;
};

struct BoxCollider2D : public IComponent
{
    BIND_OBJECT(BoxCollider2D);

    vec2 originOffset = vec2(0.0f);
    vec2 size = vec2(0.5f);
    PhysicsMaterial physMaterial;

    void* runtimeShape = nullptr;
};

struct CircleCollider : public IComponent
{
    BIND_OBJECT(CircleCollider);

    vec2 originOffset = vec2(0.0f);
    float radius = 0.5f;
    PhysicsMaterial physMaterial;

    void* runtimeShape = nullptr;
};