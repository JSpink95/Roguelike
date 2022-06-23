#include "Game\scene.h"
#include "Game\entity.h"
#include "Game\components.h"
#include "Game\physics_components.h"
#include "Game\script_component.h"

#include <box2d\b2_world.h>
#include <box2d\b2_body.h>
#include <box2d\b2_polygon_shape.h>
#include <box2d\b2_circle_shape.h>
#include <box2d\b2_fixture.h>

#include <sstream>

namespace utils
{
    b2BodyType RigidBodyTypeTpBox2D(RigidBodyType type)
    {
        switch (type)
        {
            case RigidBodyType::STATIC:
                return b2BodyType::b2_staticBody;
            case RigidBodyType::DYNAMIC:
                return b2BodyType::b2_dynamicBody;
            case RigidBodyType::KINEMATIC:
                return b2BodyType::b2_kinematicBody;
            default:
                return b2BodyType::b2_staticBody;
        }
    }
}

Scene::Ref Scene::Create(SceneSpecification const& specification)
{
    return std::make_shared<Scene>(specification);
}

Scene::Scene(SceneSpecification const& specification)
    : window(specification.window)
    , sceneManager(specification.sceneManager)
{
    if (specification.enablePhysics)
    {
        physics = std::make_shared<b2World>(b2Vec2(0.0f, -9.81f));
    }
}

Scene::~Scene()
{
}

void Scene::Begin()
{
    auto scripts = registry.view<ScriptComponent>();
    for (auto [id, scriptComponent] : scripts.each())
        scriptComponent.Invoke_OnBegin();

    if (IsPhysicsEnabled())
        InitialisePhysics();
}

void Scene::End()
{
}

void Scene::Update(float deltaTime)
{
    auto scripts = registry.view<ScriptComponent>();
    for (auto [id, scriptComponent] : scripts.each())
        scriptComponent.Invoke_OnUpdate(deltaTime);
}

void Scene::PhysicsUpdate(float deltaTime)
{
    constexpr int velocityIterations = 6;
    constexpr int positionIterations = 2;

    if (IsPhysicsEnabled())
    {
        physics->Step(deltaTime, velocityIterations, positionIterations);

        auto bodies = registry.view<RigidBodyComponent>();
        for (auto id : bodies)
        {
            auto ent = CreateEntity(id);
            auto& transform = ent->GetComponent<TransformComponent>();
            auto& rb = ent->GetComponent<RigidBodyComponent>();

            b2Body* body = (b2Body*)rb.runtimeBody;
            transform.position.x = body->GetPosition().x;
            transform.position.y = body->GetPosition().y;

            // if the body is a fixed rotation we do not want to update it here
            // this is a bit of a hack to allow us to manually set a graphical rotation for the player
            if (!body->IsFixedRotation())
            {
                transform.rotation = quat(vec3(0.0f, 0.0f, glm::degrees(body->GetAngle())));
            }
        }
    }
}

void Scene::DrawFrame(IRenderPass::Ref screenRenderPass)
{
}

void Scene::Initialize()
{
    // stub
}

void Scene::InitialisePhysics()
{
    physics->SetGravity(b2Vec2(0.0f, -9.81f * 2.0f));

    auto bodies = registry.view<RigidBodyComponent>();
    for (auto id : bodies)
    {
        auto ent = CreateEntity(id);
        auto& transform = ent->GetComponent<TransformComponent>();
        auto& rigidbody = ent->GetComponent<RigidBodyComponent>();

        b2BodyDef bodyDef;
        bodyDef.type = utils::RigidBodyTypeTpBox2D(rigidbody.type);
        bodyDef.position.Set(transform.position.x, transform.position.y);
        //bodyDef.angle = glm::radians(transform.rotation);
        bodyDef.linearDamping = rigidbody.linearDamping;
        bodyDef.angularDamping = rigidbody.angularDamping;

        auto body = physics->CreateBody(&bodyDef);
        body->SetFixedRotation(rigidbody.fixedRotation);

        rigidbody.runtimeBody = body;

        if (ent->HasComponent<BoxCollider2D>())
        {
            auto& collider = ent->GetComponent<BoxCollider2D>();

            b2PolygonShape shape;
            shape.SetAsBox(collider.size.x, collider.size.y);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = collider.physMaterial.density;
            fixtureDef.friction = collider.physMaterial.friction;
            fixtureDef.restitution = collider.physMaterial.restitution;
            fixtureDef.restitutionThreshold = collider.physMaterial.restitutionThreshold;

            auto fixture = body->CreateFixture(&fixtureDef);
            collider.runtimeShape = fixture;
        }

        if (ent->HasComponent<CircleCollider>())
        {
            auto& collider = ent->GetComponent<CircleCollider>();

            b2CircleShape shape;
            shape.m_radius = collider.radius;

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = collider.physMaterial.density;
            fixtureDef.friction = collider.physMaterial.friction;
            fixtureDef.restitution = collider.physMaterial.restitution;
            fixtureDef.restitutionThreshold = collider.physMaterial.restitutionThreshold;

            auto fixture = body->CreateFixture(&fixtureDef);
            collider.runtimeShape = fixture;
        }
    }
}

Entity::Ref Scene::CreateEntity(std::string_view tag/* = ""*/)
{
    auto ent = std::make_shared<Entity>();
    ent->id = registry.create();
    ent->world = GetSceneRef();

    std::stringstream tagStream;
    if (tag == "")
        tagStream << "entity " << (u32)ent->id;
    else
        tagStream << tag;

    ent->AddComponent<TagComponent>(tagStream.str());

    return ent;
}

Entity::Ref Scene::CreateEntity(entt::entity id)
{
    auto ent = std::make_shared<Entity>();
    ent->id = id;
    ent->world = GetSceneRef();

    return ent;
}

Entity::Ref Scene::FindEntityByTag(std::string_view tag)
{
    entt::entity foundId = entt::null;
    auto entityView = registry.view<TagComponent>();
    for (auto [id, tc] : entityView.each())
    {
        if (tc.id == tag)
        {
            foundId = id;
            break;
        }
    }

    if (foundId == entt::null)
    {
        return nullptr;
    }

    return CreateEntity(foundId);
}

void Scene::DeleteEntity(std::shared_ptr<Entity> entity)
{
    if (entity)
        registry.destroy(entity->id);
}

bool Scene::RayCast(vec2 const& origin, vec2 const& direction)
{
    struct RayCastCallback : public b2RayCastCallback
    {
        float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) final
        {
            return 0.0f;
        }
    };

    RayCastCallback callback;
    physics->RayCast(&callback, b2Vec2(origin.x, origin.y), b2Vec2(origin.x + direction.x * 1000.0f, origin.y + direction.y * 1000.0f));
    
    return false;
}