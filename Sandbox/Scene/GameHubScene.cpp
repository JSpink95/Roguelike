#include "Scene\GameHubScene.h"

#include <Core\conv.h>
#include <Core\StringUtils.h>
#include <Platform\FileIO.h>

#include <Game\Components.h>
#include <Game\physics_components.h>
#include <Game\script_component.h>

#include "Game\GameEntity.h"
#include "Scripts\PlayerController.h"

#include <stb\stb_image.h>
#include <sstream>

struct TileInfo
{
    u32 spriteIndex = 0u;
    bool valid = false;
};

std::vector<TileInfo> LoadColorValuesFromTexture(std::string_view path)
{
    std::vector<TileInfo> result = {};

    int width, height, components;

    stbi_set_flip_vertically_on_load(true);
    auto pixels = stbi_load(path.data(), &width, &height, &components, 0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            u32 index = (x + y * width) * components;

            bool valid = pixels[index + 3] != 0x0;
            result.push_back({ (u32)pixels[index] / 10u, valid });
        }
    }

    stbi_image_free(pixels);
    stbi_set_flip_vertically_on_load(false);

    return result;
}

GameHubScene::Ref GameHubScene::Create(AppWindow::Ref window, std::shared_ptr<SceneManager> sceneManager)
{
    SceneSpecification specification = {};
    specification.window = window;
    specification.enablePhysics = true;
    specification.sceneManager = sceneManager;

    auto result = std::make_shared<GameHubScene>(specification);

    if (!result->InitialiseRenderPass(specification.window->graphics))
    {
        return nullptr;
    }

    if (!result->InitialiseRenderers(specification.window->graphics))
    {
        return nullptr;
    }

    if (!result->InitialiseTileGrid())
    {
        return nullptr;
    }

    return result;
}

GameHubScene::GameHubScene(SceneSpecification const& specification)
    : Super(specification)
{

}

bool GameHubScene::InitialiseRenderPass(IGraphicsDevice::Ref graphics)
{
    RenderPassSpecification tileGridRenderPassSpec = {};
    tileGridRenderPassSpec.clearColor = vec4(1.0f, 0.0f, 1.0f, 0.0f);
    tileGridRenderPassSpec.size = ivec2(1024);
    tileGridRenderPassSpec.attachments = {
        { RenderPassSpecification::Attachment::Format::RGBA, RenderPassSpecification::Attachment::Type::COLOR, 1u },
        { RenderPassSpecification::Attachment::Format::RED, RenderPassSpecification::Attachment::Type::DEPTH, 1u },
    };

    renderPass = graphics->CreateRenderPass(tileGridRenderPassSpec);
    renderTarget = renderPass->CreateFramebuffer();

    return true;
}

bool GameHubScene::InitialiseRenderers(IGraphicsDevice::Ref graphics)
{
    TextureSpecification textureSpec = {};
    textureSpec.filterMode = TextureFilter::POINT;
    textureSpec.wrapMode = TextureWrap::REPEAT;

    // renderer
    {
        SpriteSheetSpecification spriteSheetSpec = {};
        spriteSheetSpec.spriteSheetSizePixels = ivec2(256);
        spriteSheetSpec.spriteSizePixels = ivec2(16);

        SpriteRendererSpecification rendererSpec = {};
        rendererSpec.flipTextureCoords = true;
        rendererSpec.textureSize = ivec2(256);
        rendererSpec.spriteSize = ivec2(16);
        rendererSpec.textures = {
            { 0u, graphics->CreateTexture("assets/textures/sprites0.png", textureSpec) },
            { 1u, graphics->CreateTexture("assets/textures/tileset_defaultlayer.png", textureSpec) },
            { 2u, graphics->CreateTexture("assets/textures/tileset_foregroundlayer.png", textureSpec) },
        };

        renderer = SpriteRenderer::Create(graphics, rendererSpec);
    }

    // imageRenderer
    {
        SpriteRendererSpecification rendererSpec = {};
        rendererSpec.flipTextureCoords = true;
        rendererSpec.textureSize = ivec2(1024);
        rendererSpec.spriteSize = ivec2(1024);
        rendererSpec.textures = {
            { 0u, renderTarget->GetColorAttachment(FramebufferSpecification::Attachment::Index::FIRST) },
        };

        imageRenderer = SpriteRenderer::Create(graphics, rendererSpec);
    }

    // textRenderer
    {
        SpriteRendererSpecification rendererSpec = {};
        rendererSpec.textureSize = ivec2(256, 32);
        rendererSpec.spriteSize = ivec2(16);
        rendererSpec.textures = {
            { 0u, graphics->CreateTexture("assets/textures/fonts/ancient.png", textureSpec) },
            { 1u, graphics->CreateTexture("assets/textures/fonts/regular_en.png", textureSpec) },
        };

        textRenderer = TextRenderer::Create(graphics, rendererSpec);
    }

    // just initialise the debug renderer
    debugRenderer = DebugRenderer::Create(graphics);

    return true;
}

bool GameHubScene::InitialiseTileGrid()
{
    TileGridSpecification tileGridSpec = {};
    tileGridSpec.width = 32u;
    tileGridSpec.height = 16u;

    tileGrid = TileGrid::Create(tileGridSpec);

    auto background = LoadColorValuesFromTexture("assets/textures/gamehub/hub_level_defaultlayer.png");
    auto foreground = LoadColorValuesFromTexture("assets/textures/gamehub/hub_level_foregroundlayer.png");
    auto collision = LoadColorValuesFromTexture("assets/textures/gamehub/hub_level_collisionlayer.png");
    
    tileGrid->ForEachTileInLayer([this, &background](Tile::Ref tile) -> void
    {
        u32 index = tile->x + tile->y * 32u;
        if (background.at(index).valid)
        {
            tile->sprite = Sprite::Create(renderer->spriteSheets.at(1), background.at(index).spriteIndex);
        }
    }, TileGridLayerIndex::Default);
    
    tileGrid->ForEachTileInLayer([this, &foreground](Tile::Ref tile) -> void
    {
        u32 index = tile->x + tile->y * 32u;
        if (foreground.at(index).valid)
        {
            tile->sprite = Sprite::Create(renderer->spriteSheets.at(2), foreground.at(index).spriteIndex);
        }
    }, TileGridLayerIndex::Foreground);
    
    return true;
}

void GameHubScene::Initialize()
{
    Super::Initialize();

    screenCamera = CreateEntity("Screen Camera");
    {
        OrthoCameraSpecification orthoCameraSpec = {};
        orthoCameraSpec.flipViewport = true;
        orthoCameraSpec.aspectRatio = window->GetAspectRatio();
        orthoCameraSpec.depth = 5.0f;
        orthoCameraSpec.size = (float)window->GetDimensions().x;
        orthoCameraSpec.origin = OrthoCameraSpecification::Origin::BottomLeft;

        screenCamera->AddComponent<CameraComponent>(orthoCameraSpec);
        screenCamera->AddComponent<TransformComponent>();
    }

    camera = CreateEntity("Main Camera");
    {
        OrthoCameraSpecification orthoCameraSpec = {};
        orthoCameraSpec.aspectRatio = 1.0f;
        orthoCameraSpec.depth = 5.0f;
        orthoCameraSpec.size = 8.0f;

        auto& cameraComponent = camera->AddComponent<CameraComponent>(orthoCameraSpec);
        auto& transformComponent = camera->AddComponent<TransformComponent>();
        transformComponent.position = vec3(4.0f, 4.0f, 0.0f);
    }

    player = CreateSpriteEntity(GetSceneRef(), Sprite::Create(renderer->GetSpriteSheet(0), 0), true, "Player");
    {
        auto& transform = player->GetComponent<TransformComponent>();
        transform.position = vec3(2.0f, 3.0f, 0.0f);

        auto& rigidBody = player->GetComponent<RigidBodyComponent>();
        rigidBody.type = RigidBodyType::DYNAMIC;
        rigidBody.fixedRotation = true;
        rigidBody.linearDamping = 1.0f;

        auto& circle = player->AddComponent<CircleCollider>();
        circle.radius = 0.5f;
        circle.physMaterial.density = 5.0f;
        circle.physMaterial.friction = 10.0f;

        auto& scripts = player->AddComponent<ScriptComponent>();
        scripts.Add<PlayerController>(window->keyboard);
    }

    // spawn the level collision
    auto collisionData = std::istringstream(core::fileio::LoadFileToString("assets/level/gamehub.collision"));

    std::string line;
    while (std::getline(collisionData, line))
    {
        if (core::string::StartWith(line, "#rect"))
        {
            auto rectData = core::string::convert<ivec4>(line.substr(6));
            Entity::Ref entity = CreatePhysicsEntity(GetSceneRef());

            auto& transform = entity->GetComponent<TransformComponent>();
            transform.position.x = (float)rectData.x;
            transform.position.y = (float)rectData.y;

            auto& rigidBody = entity->GetComponent<RigidBodyComponent>();
            rigidBody.type = RigidBodyType::STATIC;

            auto& box = entity->AddComponent<BoxCollider2D>();
            box.size = vec2((float)rectData.z, (float)rectData.w);

            collisionBoxes.push_back(entity);
        }
    }
}

void GameHubScene::Begin()
{
    Super::Begin();
}

void GameHubScene::Update(float deltaTime)
{
    Super::Update(deltaTime);

    player->GetComponent<ScriptComponent>().Find<PlayerController>()->OnUpdate(deltaTime);

    auto& playerTransform = player->GetComponent<TransformComponent>();
    auto& cameraTransform = camera->GetComponent<TransformComponent>();

    cameraTransform.position = playerTransform.position;
    cameraTransform.position.x = glm::clamp(cameraTransform.position.x, 4.0f, 28.0f);
    cameraTransform.position.y = glm::clamp(cameraTransform.position.y, 4.0f, 28.0f);
}

void GameHubScene::DrawFrame(IRenderPass::Ref screenRenderPass)
{
    Super::DrawFrame(screenRenderPass);

    renderPass->Begin(renderTarget);

    renderer->Begin(camera);

    auto& cameraTransform = camera->GetComponent<TransformComponent>();
    TileGridBounds bounds = { 0, 0, 17, 17 }; // give a one tile buffer so they don't "pop" into existence
    bounds.x = glm::floor(glm::max(cameraTransform.position.x - 4.0f, 0.0f));
    bounds.y = glm::floor(glm::max(cameraTransform.position.y - 4.0f, 0.0f));
    
    TileGridSpan grid(tileGrid, bounds);
    grid.ForEachTileInLayer([this](Tile::Ref tile)
    {
        renderer->SubmitSprite(
              (float)tile->x
            , (float)tile->y
            , 0.0f              // background layer depth
            , 1.0f
            , tile->sprite);
    
    }, TileGridLayerIndex::Default);

    grid.ForEachTileInLayer([this](Tile::Ref tile)
    {
        renderer->SubmitSprite(
              (float)tile->x
            , (float)tile->y
            , 1.0f              // foreground layer depth
            , 1.0f
            , tile->sprite);
    
    }, TileGridLayerIndex::Foreground);

    auto spriteEntities = registry.view<TransformComponent, SpriteComponent>();
    for (auto [id, transform, sprite] : spriteEntities.each())
    {
        // no sprite!
        if (sprite.sprite == nullptr)
            continue;

        renderer->SubmitSprite(
              transform.position.x - 0.5f
            , transform.position.y - 0.5f
            , 1.0f
            , 1.0f
            , sprite.sprite);
    }

    renderer->Finish();

#if 1
    debugRenderer->OpenVertexStream();

    auto entitiesWithCollision = registry.view<TransformComponent, RigidBodyComponent>();
    for (auto [id, transform, rigidbody] : entitiesWithCollision.each())
    {
        auto entity = CreateEntity(id);
        if (entity->HasComponent<BoxCollider2D>())
        {
            auto& box = entity->GetComponent<BoxCollider2D>();
            debugRenderer->DrawBox2D(vec2(transform.position), vec2(transform.position) + box.size, vec4(1.0f, 0.0f, 1.0f, 1.0f));
        }

        if (entity->HasComponent<CircleCollider>())
        {
            auto& circle = entity->GetComponent<CircleCollider>();
            debugRenderer->DrawCircle2D(vec2(transform.position), circle.radius, vec4(1.0f, 0.0f, 1.0f, 1.0f));
        }
    }

    debugRenderer->CloseVertexStream();
#endif

    renderPass->End();

    //{
    //    ivec2 windowSize = window->GetDimensions();
    //    PerFrameMatrixShaderBlock perFrameMatrices = {
    //        glm::ortho(0.0f, (float)windowSize.x, (float)windowSize.y, 0.0f, -5.0f, 5.0f)
    //    };
    //    PerObjectMatrixShaderBlock perObjectMatrices = { mat4(1.0f) };
    //
    //    window->graphics->GetPerFrameShaderTransferBlock()->Set(sizeof(PerFrameMatrixShaderBlock), &perFrameMatrices);
    //    window->graphics->GetPerObjectShaderTransferBlock()->Set(sizeof(PerObjectMatrixShaderBlock), &perObjectMatrices);
    //}

    float renderSize = 512.0f;

    screenRenderPass->Begin(nullptr); // maybe the begin function of a render pass should setup the matrix shader shit

    imageRenderer->Begin(screenCamera);
    imageRenderer->SubmitSprite((1920.0f - renderSize) / 2.0f, (1080.0f - renderSize) / 2.0f, 0.0f, vec2(renderSize), 0, 0);
    imageRenderer->Finish();

    textRenderer->Begin(screenCamera);

    constexpr float TextSize = 50.0f;
    textRenderer->SubmitText("HELLO WORLD", TextSize, 200.0f, 0.0f, TextSize, 0);
    textRenderer->SubmitText("HELLO WORLD", TextSize, 200.0f - TextSize - 10.0f, 0.0f, TextSize, 1);

    textRenderer->Finish();

    screenRenderPass->End();
}

void GameHubScene::End()
{
    Super::End();

    DeleteEntity(camera);
    DeleteEntity(player);
    for (auto entity : collisionBoxes)
        DeleteEntity(entity);
}
