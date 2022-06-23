#pragma once
#include "GameCore.h"
#include <Core\Object.h>

#include "Game\Scene.h"

// very simple FiniteStateMachine for scene management
struct SceneStateMachine : public Object
{
    BIND_OBJECT(SceneStateMachine);

    static Ref Create(std::shared_ptr<SceneManager> sceneManager)
    {
        return std::make_shared<SceneStateMachine>(sceneManager);
    }

    SceneStateMachine(std::shared_ptr<SceneManager> sceneManager);

    void BeginScene(Scene::Ref newScene);
    void EndScene(Scene::Ref scene);

    void EndActiveScene();

    void UpdateScene(float deltaTime);
    void UpdatePhysicsScene(float fixedDeltaTime);
    void DrawScene(IRenderPass::Ref screenRenderPass);

    Scene::Ref activeScene = nullptr;
    std::shared_ptr<SceneManager> sceneManager = nullptr;
};

struct SceneTransition : public Object
{
    BIND_OBJECT(SceneTransition);

    static Ref Create(Scene::Ref newScene)
    {
        return std::make_shared<SceneTransition>(newScene);
    }

    SceneTransition(Scene::Ref newScene);

    Scene::Ref newScene = nullptr;
};

struct SceneManager : public Object
{
    BIND_OBJECT(SceneManager);

    static Ref Create();

    SceneManager() = default;

    bool Initialise();
    void QueueSceneTranstition(SceneTransition::Ref transition);
    void Update(float deltaTime);
    void PhysicsUpdate(float deltaTime);
    void Draw(IRenderPass::Ref screenRenderPass);

    Scene::Ref GetPreparedScene(u32 index);
    void PushPrebuiltScene(Scene::Ref scene);

    SceneStateMachine::Ref sceneStateMachine = nullptr;
    std::vector<SceneTransition::Ref> queuedTransitions = {};
    std::vector<Scene::Ref> preparedScenes = {};
};