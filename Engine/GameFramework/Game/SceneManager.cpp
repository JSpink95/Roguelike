#include "Game\SceneManager.h"

SceneStateMachine::SceneStateMachine(std::shared_ptr<SceneManager> sceneManager)
    : sceneManager(sceneManager)
{

}

void SceneStateMachine::BeginScene(Scene::Ref newScene)
{
    EndActiveScene();

    if (newScene)
    {
        activeScene = newScene;
        activeScene->sceneManager = sceneManager;
        activeScene->Initialize();
        activeScene->Begin();
    }
}

void SceneStateMachine::EndScene(Scene::Ref scene)
{
    scene->End();
}

void SceneStateMachine::EndActiveScene()
{
    if (activeScene)
        EndScene(activeScene);

    activeScene = nullptr;
}

void SceneStateMachine::UpdateScene(float deltaTime)
{
    if (activeScene)
        activeScene->Update(deltaTime);
}

void SceneStateMachine::UpdatePhysicsScene(float fixedDeltaTime)
{
    if (activeScene)
        activeScene->PhysicsUpdate(fixedDeltaTime);
}

void SceneStateMachine::DrawScene(IRenderPass::Ref screenRenderPass)
{
    if (activeScene)
        activeScene->DrawFrame(screenRenderPass);
}

SceneTransition::SceneTransition(Scene::Ref newScene)
    : newScene(newScene)
{

}

SceneManager::Ref SceneManager::Create()
{
    auto result = std::make_shared<SceneManager>();

    if (!result->Initialise())
    {
        return nullptr;
    }

    return result;
}

bool SceneManager::Initialise()
{
    sceneStateMachine = SceneStateMachine::Create(GetSceneManagerRef());
    return true;
}

void SceneManager::QueueSceneTranstition(SceneTransition::Ref transition)
{
    queuedTransitions.push_back(transition);
}

void SceneManager::Update(float deltaTime)
{
    // update the scene
    sceneStateMachine->UpdateScene(deltaTime);

    if (!queuedTransitions.empty())
    {
        auto transition = queuedTransitions.front();
        queuedTransitions.erase(queuedTransitions.begin());

        sceneStateMachine->BeginScene(transition->newScene);
    }
}

void SceneManager::PhysicsUpdate(float deltaTime)
{
    sceneStateMachine->UpdatePhysicsScene(deltaTime);
}

void SceneManager::Draw(IRenderPass::Ref screenRenderPass)
{
    sceneStateMachine->DrawScene(screenRenderPass);
}

Scene::Ref SceneManager::GetPreparedScene(u32 index)
{
    if (index > preparedScenes.size())
        return nullptr;

    return preparedScenes.at(index);
}

void SceneManager::PushPrebuiltScene(Scene::Ref scene)
{
    preparedScenes.push_back(scene);
}