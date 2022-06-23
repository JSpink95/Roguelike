#pragma once
#include <Core.h>
#include <Core\Object.h>
#include <Core\timer.h>
#include <Graphics\GraphicsAPI.h>

#include <Game\Scene.h>
#include <Render\SpriteRenderer.h>

struct SplashScreenScene : public Scene
{
    BIND_OBJECT(SplashScreenScene);

    static Ref Create(AppWindow::Ref window, std::shared_ptr<SceneManager> sceneManager);

    SplashScreenScene(SceneSpecification const& specification);

    void Initialize() final;
    void Begin() final;
    void End() final;
    void Update(float deltaTime) final;
    void DrawFrame(IRenderPass::Ref screenRenderPass) final;

    void TransitionToNextScene();

    EventTimer::Ref splashTransitionTimer = nullptr;

    SpriteRenderer::Ref renderer = nullptr;
    Entity::Ref camera = nullptr;
    Sprite::Ref sprites[2];
    u32 spriteIndex = 0u;
};
