#pragma once
#include <Core.h>
#include <Core\Object.h>
#include <Graphics\GraphicsAPI.h>

#include <Game\Entity.h>
#include <Game\Scene.h>
#include <Render\TextRenderer.h>
#include <Render\SpriteRenderer.h>
#include <Render\DebugRenderer.h>

#include "Tile\TileGrid.h"

struct GameHubScene : public Scene
{
    BIND_OBJECT(GameHubScene);

    static Ref Create(AppWindow::Ref window, std::shared_ptr<SceneManager> sceneManager);

    GameHubScene(SceneSpecification const& specification);

    void Begin() final;
    void Update(float deltaTime) final;
    void DrawFrame(IRenderPass::Ref screenRenderPass) final;
    void End() final;
    void Initialize() final;

    SpriteRenderer::Ref renderer = nullptr;
    SpriteRenderer::Ref imageRenderer = nullptr;
    TextRenderer::Ref textRenderer = nullptr;
    DebugRenderer::Ref debugRenderer = nullptr;
    IRenderPass::Ref renderPass = nullptr;
    IFramebuffer::Ref renderTarget = nullptr;

    // Entities
    Entity::Ref screenCamera = nullptr;
    Entity::Ref camera = nullptr;
    Entity::Ref player = nullptr;
    std::vector<Entity::Ref> collisionBoxes = {};
    TileGrid::Ref tileGrid = nullptr;
private:
    bool InitialiseRenderPass(IGraphicsDevice::Ref graphics);
    bool InitialiseRenderers(IGraphicsDevice::Ref graphics);
    bool InitialiseTileGrid();
};
