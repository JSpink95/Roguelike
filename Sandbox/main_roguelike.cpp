#include <Core.h>
#include <Core\Object.h>
#include <Core\Window.h>
#include <Core\Timer.h>

#include <Game\Entity.h>
#include <Game\Scene.h>
#include <Game\SceneManager.h>
#include <Game\Components.h>
#include <Game\physics_components.h>
#include <Game\script_component.h>

#include <Render\DebugRenderer.h>
#include <Render\SpriteRenderer.h>

#include "Tile\TileGrid.h"
#include "Tile\TileGridRenderer.h"
#include "Tile\TileGridGameScene.h"

#include "Scene\GameScene.h"

// TODO(jack): turn this into a generic all purpose tick dispatcher
struct PhysicsTickDispatcher : public Object
{
    static constexpr double TickRate = 1.0 / 60.0;
    using PhysicsTickListener = std::function<void(double)>;

    BIND_OBJECT(PhysicsTickDispatcher);

    static Ref Create();

    void Update(double deltaTime);

    double tickTime = 0.0;
    std::vector<PhysicsTickListener> listeners = {};
};

PhysicsTickDispatcher::Ref PhysicsTickDispatcher::Create()
{
    return std::make_shared<PhysicsTickDispatcher>();
}

void PhysicsTickDispatcher::Update(double deltaTime)
{
    tickTime += deltaTime;
    if (tickTime >= TickRate)
    {
        u32 physicsUpdatesThisFrame = 0;
        while (tickTime >= TickRate)
        {
            // dispatch physics tick
            for (auto listener : listeners)
                listener(TickRate);

            tickTime -= TickRate;
            ++physicsUpdatesThisFrame;
        }
    }
}

int main()
{
    AppWindowSpecification windowSpec = {};
    windowSpec.width = 1920;
    windowSpec.height = 1080;
    windowSpec.title = "Roguelike";
    windowSpec.api = EGraphicsAPI::OPENGL;

    auto window = AppWindow::Create(windowSpec);

    RenderPassSpecification defaultRenderPassSpec = {};
    defaultRenderPassSpec.clearColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    defaultRenderPassSpec.size = window->GetDimensions();
    defaultRenderPassSpec.attachments = {
        { RenderPassSpecification::Attachment::Format::RGBA, RenderPassSpecification::Attachment::Type::COLOR, 1u },
    };

    auto defaultRenderPass = window->graphics->CreateRenderPass(defaultRenderPassSpec);

    Timer frameTimer(true);

    constexpr double FrameTimeStep = 1.0 / 60.0;

    auto sceneManager = SceneManager::Create();
    PopulateSceneManager(sceneManager, window);

    sceneManager->QueueSceneTranstition(SceneTransition::Create(sceneManager->GetPreparedScene((u32)GameScene::SplashScreen)));

    auto physicsTickDispatcher = PhysicsTickDispatcher::Create();
    physicsTickDispatcher->listeners.push_back([&sceneManager](double deltaTime) -> void
    {
        sceneManager->PhysicsUpdate(deltaTime);
    });

    while (!window->closeRequested)
    {
        // start the timer
        double deltaTime = frameTimer.Mark();
        physicsTickDispatcher->Update(deltaTime);

        sceneManager->Update((float)deltaTime);
        sceneManager->Draw(defaultRenderPass);

        window->PollEvents();
        window->Present();

        while (frameTimer.ElapsedTime() < FrameTimeStep);
    }

    return 0;
}