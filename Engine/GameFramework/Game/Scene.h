#pragma once
#include <Core.h>
#include <Core\Object.h>
#include <Core\Window.h>
#include <Graphics\GraphicsAPI.h>
#include <entt.hpp>

#include <unordered_map>

struct Entity;
struct SceneManager;

class b2World;

struct SceneSpecification
{
    AppWindow::Ref window = nullptr;
    std::shared_ptr<SceneManager> sceneManager = nullptr;
    bool enablePhysics = false;
};

struct Scene : public Object
{
    BIND_OBJECT(Scene);

    static Ref Create(SceneSpecification const& specification);

    Scene(SceneSpecification const& specification);
    virtual ~Scene();

    virtual void Begin();
    virtual void End();
    virtual void Update(float deltaTime);
    virtual void PhysicsUpdate(float deltaTime);
    virtual void DrawFrame(IRenderPass::Ref screenRenderPass);

    virtual void Initialize();

    inline bool IsPhysicsEnabled() const { return physics != nullptr; }

    std::shared_ptr<Entity> CreateEntity(std::string_view tag = "");
    std::shared_ptr<Entity> CreateEntity(entt::entity id);
    std::shared_ptr<Entity> FindEntityByTag(std::string_view tag);
    void DeleteEntity(std::shared_ptr<Entity> ent);

    bool RayCast(vec2 const& origin, vec2 const& direction);

    entt::registry registry;
    AppWindow::Ref window = nullptr;
    std::shared_ptr<SceneManager> sceneManager = nullptr;
    std::shared_ptr<b2World> physics = nullptr;
private:
    void InitialisePhysics();
};
