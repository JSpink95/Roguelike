#include "Scene\SplashScreenScene.h"
#include <Game\SceneManager.h>
#include <Game\Components.h>

#include "Scene\GameScene.h"

constexpr float SplashScreenTimerDuration = 1.0f;

SplashScreenScene::Ref SplashScreenScene::Create(AppWindow::Ref window, std::shared_ptr<SceneManager> sceneManager)
{
    SceneSpecification specification = {};
    specification.window = window;
    specification.sceneManager = sceneManager;

    return std::make_shared<SplashScreenScene>(specification);
}

SplashScreenScene::SplashScreenScene(SceneSpecification const& specification)
    : Scene(specification)
{
    TextureSpecification textureSpecification = {};
    textureSpecification.filterMode = TextureFilter::POINT;
    textureSpecification.wrapMode = TextureWrap::CLAMP;

    SpriteRendererSpecification rendererSpecification = {};
    rendererSpecification.flipTextureCoords = true;
    rendererSpecification.textureSize = ivec2(480, 270);
    rendererSpecification.spriteSize = ivec2(480, 270);
    rendererSpecification.textures = {
        { 0u, window->graphics->CreateTexture("assets/textures/splashscreen/title.png", textureSpecification) },
        { 1u, window->graphics->CreateTexture("assets/textures/splashscreen/credits.png", textureSpecification) },
    };

    renderer = SpriteRenderer::Create(window->graphics, rendererSpecification);

    sprites[0] = renderer->CreateSprite(0, 0);
    sprites[1] = renderer->CreateSprite(1, 0);

    splashTransitionTimer = EventTimer::Create(false, SplashScreenTimerDuration, [this]() -> bool
    {
        spriteIndex += 1;

        if (spriteIndex <= 1)
        {
            splashTransitionTimer->StartTimer();
            return false;
        }

        TransitionToNextScene();
        return true;
    });
}

void SplashScreenScene::Initialize()
{
    Super::Initialize();

    camera = CreateEntity("Main Camera");
    {
        OrthoCameraSpecification orthoCameraSpec = {};
        orthoCameraSpec.aspectRatio = window->GetAspectRatio();
        orthoCameraSpec.depth = 5.0f;
        orthoCameraSpec.size = (float)window->GetDimensions().x;
        orthoCameraSpec.origin = OrthoCameraSpecification::Origin::BottomLeft;

        camera->AddComponent<CameraComponent>(orthoCameraSpec);
        camera->AddComponent<TransformComponent>();
    }
}

void SplashScreenScene::Begin()
{
    Super::Begin();

    splashTransitionTimer->StartTimer();
}

void SplashScreenScene::End()
{
    Super::End();
}

void SplashScreenScene::Update(float deltaTime)
{
    Super::Update(deltaTime);

    splashTransitionTimer->Update(deltaTime);
}

void SplashScreenScene::DrawFrame(IRenderPass::Ref screenRenderPass)
{
    Super::DrawFrame(screenRenderPass);

    ivec2 windowSize = window->GetDimensions();
    //{
    //    PerFrameMatrixShaderBlock perFrameMatrices = {
    //        glm::ortho(0.0f, (float)windowSize.x, (float)windowSize.y, 0.0f, -1.0f, 1.0f)
    //    };
    //    PerObjectMatrixShaderBlock perObjectMatrices = { mat4(1.0f) };
    //
    //    window->graphics->GetPerFrameShaderTransferBlock()->Set(sizeof(PerFrameMatrixShaderBlock), &perFrameMatrices);
    //    window->graphics->GetPerObjectShaderTransferBlock()->Set(sizeof(PerObjectMatrixShaderBlock), &perObjectMatrices);
    //}

    Sprite::Ref activeSprite = sprites[spriteIndex];
    float startAlpha = glm::smoothstep(0.0f, 0.5f, (float)splashTransitionTimer->ElapsedTime());
    float endAlpha = 1.0f - glm::smoothstep(SplashScreenTimerDuration - 0.5f, SplashScreenTimerDuration, (float)splashTransitionTimer->ElapsedTime());

    float alpha = glm::min(startAlpha, endAlpha);

    screenRenderPass->Begin(nullptr);

    renderer->Begin(camera);
    {
        renderer->SubmitSprite(0.0f, 0.0f, 0.0f, vec2(windowSize), activeSprite, alpha);
    }
    renderer->Finish();

    screenRenderPass->End();
}

void SplashScreenScene::TransitionToNextScene()
{
    SceneTransition::Ref mainGameScene = SceneTransition::Create(sceneManager->GetPreparedScene(static_cast<u32>(GameScene::GameHub)));
    sceneManager->QueueSceneTranstition(mainGameScene);
}