#include <Core.h>

#include <Core\window.h>
#include <Core\KeyboardInputManager.h>

#include <Platform\FileIO.h>

#include <Asset\assets.h>
#include <Asset\AssetManager.h>

#include <Graphics\GraphicsAPI.h>
#include <Graphics\Material.h>
#include <Graphics\vertex.h>
#include <Graphics\Framebuffer.h>

#include <GameCore.h>
#include <Game\scene.h>
#include <Game\entity.h>
#include <Game\components.h>
#include <Game\script_component.h>
#include <Game\physics_components.h>
#include <Game\Sprite.h>
#include <Game\game_meshes.h>
#include <Game\System\ScriptSystem.h>

#include "Game\Scripts\PlayerController.h"
#include "Game\Scripts\CameraController.h"
#include "Game\Scripts\Door.h"
#include "Game\Scripts\RotatingObject.h"

#include <fstream>
#include <filesystem>
#include <rapidxml.hpp>
#include <random>

static std::default_random_engine random_engine;
static std::uniform_int_distribution<int> random_distribution(0, 100);

int main()
{
#if 0
    random_engine.seed(std::time(0));

    auto window = AppWindow::Create(L"Roguelike", 640 * 2, 480 * 2);
    {
        QuitListener quitHandler;
        window->RegisterQuitListener(&quitHandler);

        //glEnable(GL_DEPTH_TEST);

        ITexture::InitialiseDefaultTextures(window->graphics);
        Mesh::Initialise(window->graphics);

        // load Framebuffers
        FramebufferSpecification main_fb_spec = {};
        main_fb_spec.dimensions = ivec2(240, 180);
        main_fb_spec.attachments.push_back({ ColorFormat::RGBA, TextureFilter::POINT });
        main_fb_spec.enableDepth = true;
        auto main_Framebuffer = window->graphics->CreateFramebuffer(main_fb_spec);
        //main_Framebuffer->desktop_window = window;

        auto assets = std::make_shared<AssetManager>(window->graphics);
        core::asset::ParseAssetDirectory(assets);

        auto screenMaterial = window->graphics->CreateMaterial(assets->FindAsset<IShaderProgram>("assets/bin/screen_shader.asset"));
        screenMaterial->SetTexture("_Texture", main_Framebuffer->GetColorAttachment(FramebufferAttachmentID::ALBEDO));

        auto game_scene = Scene::Create();

        auto player_entity = game::CreateSprite(
            game_scene
            , assets->FindAsset<Sprite>("assets/bin/player_sprite.asset")
            , "player"
            , vec3(4.0f, 5.5f, 4.0f));
        {
            player_entity->AddComponent<ScriptComponent>().Add<PlayerController>(window);
            
            auto& rb = player_entity->AddComponent<RigidBodyComponent>();
            rb.fixed_rotation = true;
            rb.type = RigidBodyType::DYNAMIC;
            rb.linear_damping = 5.0f;

            auto& circle = player_entity->AddComponent<CircleCollider>();
            circle.radius = 0.45f;
            circle.phys_mat.friction = 10.0f;
            //circle.phys_mat.restitution = 0.0f;
            //circle.phys_mat.restitution_threshold = 0.0f;
            circle.phys_mat.density = 10.0f;
        }

        auto camera_entity = game_scene->CreateEntity("camera");
        {
            auto& transform = camera_entity->AddComponent<TransformComponent>(vec3(0.0f), vec3(1.0f), 45.0f);
            auto& camera = camera_entity->AddComponent<CameraComponent>(10.0f, 50.0f);
            camera_entity->AddComponent<ScriptComponent>().Add<CameraController>(window);
        }

        auto floor_sprite = assets->FindAsset<Sprite>("assets/bin/FloorCentrePattern_strip16_sprite.asset");
        auto wall_sprite = assets->FindAsset<Sprite>("assets/bin/WallCentreBrick_strip16_sprite.asset");

        auto floor_grid_entity = game_scene->CreateEntity("floor_grid_collection");
        {
            auto& sgc = floor_grid_entity->AddComponent<SpriteGridComponent>();
            sgc.collisionEnabled = false;
            sgc.grid = std::make_shared<SpriteGrid>(game_scene, floor_sprite);
            sgc.grid->SetDepth(-15.0f);
        }

        auto wall_grid_entity = game_scene->CreateEntity("wall_grid_collection");
        {
            auto& sgc = wall_grid_entity->AddComponent<SpriteGridComponent>();
            sgc.collisionEnabled = true;
            sgc.grid = std::make_shared<SpriteGrid>(game_scene, nullptr);

            for (u64 x = 0; x < SpriteGrid::WIDTH; ++x)
            {
                for (u64 y = 0; y < SpriteGrid::HEIGHT; ++y)
                {
                    // make room for the door!
                    if (x == 7 && y == 15)
                        continue;

                    if ((x > 0 && x < 15) && (y > 0 && y < 15) && random_distribution(random_engine) > 10)
                        continue;

                    sgc.grid->SetTile(x, y, game_scene, wall_sprite);
                }
            }

            sgc.grid->SetCollisionEnabled(true);
        }

        auto crate = game::CreateSprite(
            game_scene
            , assets->FindAsset<Sprite>("assets/bin/Crate_strip8_sprite.asset")
            , ""
            , vec3(6.0f, 8.0f, 0.0f));
        
        {
            auto& rb = crate->AddComponent<RigidBodyComponent>();
            rb.fixed_rotation = false;
            rb.linear_damping = 10.0f;
            rb.angular_damping = 12.0f;
            rb.type = RigidBodyType::DYNAMIC;

            auto& collider = crate->AddComponent<BoxCollider2D>();
            collider.phys_mat.density = 5.0f;
            collider.size = vec2(0.45f);
        }

        auto door = game::CreateSprite(
            game_scene
            , assets->FindAsset<Sprite>("assets/bin/TwoSidedDoor_strip16_sprite.asset")
            , "door"
            , vec3(9.0f, 2.0f, 0.0f));

        door->AddComponent<ScriptComponent>().Add<DoorScript>();
        door->GetComponent<TransformComponent>().rotation = 90.0f;
        door->GetComponent<TransformComponent>().anchorPoint = vec3(-0.5f, 0.0f, 0.0f);

        game::CreateSprite(
            game_scene
            , assets->FindAsset<Sprite>("assets/bin/LockedDoor_strip16_sprite.asset")
            , "locked_door"
            , vec3(7.0f, 15.0f, 0.0f)
            , 180.0f);

        auto key = game::CreateSprite(
            game_scene
            , assets->FindAsset<Sprite>("assets/bin/key_item_sprite.asset")
            , ""
            , vec3(6.0f, 4.0f, 0.0f));

        key->AddComponent<ScriptComponent>().Add<RotatingObject>(45.0f);

        float time = 0.0f;
        float aspect = window->GetAspectRatio();

        auto script_handler = std::make_shared<ScriptSystem>(game_scene);
        script_handler->SetState(ScriptSystem::State::BEGIN);

        script_handler->Update(0.0f);
        script_handler->SetState(ScriptSystem::State::UPDATE);

        game_scene->Begin();

        auto global_renderer_data = std::make_shared<RendererData>(window->graphics);
        auto renderer = std::make_shared<SpriteRenderer>(global_renderer_data, window->graphics);

        while (!quitHandler.wantsToQuit)
        {
            constexpr float deltaTime = 0.016666667f;
            time += deltaTime;

            script_handler->Update(deltaTime);
            game_scene->Update(deltaTime);

            // gather all sprites
            auto sprite_collection = game_scene->registry.view<SpriteComponent, TransformComponent>();

            // disable tiles outside of "radius"

            constexpr float lit_radius = 5.0f;
            auto& player_transform = player_entity->GetComponent<TransformComponent>();
            for (auto [id, sc, tc] : sprite_collection.each())
            {
                // perform distance check ignoring Z
                float dist = game::CalculateDistance(player_transform, tc);
                sc.enabled = (dist < lit_radius);
                sc.litValue = glm::smoothstep(0.0f, 1.0f, 1.0f - (dist / lit_radius));
            }

            main_Framebuffer->Begin();

            renderer->BeginFrame(game_scene, camera_entity->GetComponent<ScriptComponent>().Find<CameraController>()->cameraTilt);

            for (auto [id, sc, tc] : sprite_collection.each())
            {
                if (sc.enabled)
                {
                    renderer->Submit(game_scene->CreateEntity(id));
                }
            }

            renderer->EndFrame();

            main_Framebuffer->End();

            window->graphics->SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            window->graphics->ClearFrame(ClearFlags::COLOR | ClearFlags::DEPTH);

            Mesh::screenQuad->Submit(DrawMode::TRIANGLES, screenMaterial);

            window->PollEvents();
            window->SwapBuffers();
        }

        script_handler->SetState(ScriptSystem::State::FINISH);
        script_handler->Update(0.0f);
        game_scene->Finish();

        game_scene = nullptr;

        Mesh::Deinitialise();

        window->UnregisterQuitListener(&quitHandler);
    }

    core::destroy_window(window);
    window = nullptr;
#endif
}