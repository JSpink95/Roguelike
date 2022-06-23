#include <Core.h>
#include <Core\window.h>

#include <Graphics\GraphicsAPI.h>

#include <Game\Scene.h>
#include <Game\Entity.h>
#include <Game\Components.h>

#include <imgui\imgui.h>
#include <imgui\imgui_impl_opengl3.h>
#include <imgui\imgui_impl_sdl.h>

#define COMPONENT_MENU_ITEM(Comp)\
if (!entity->HasComponent<Comp>())\
{\
    if (ImGui::MenuItem(#Comp)) entity->AddComponent<Comp>();\
}

struct SelectionContext: public Object
{
    BIND_OBJECT(SelectionContext);

    static Ref Create(int index, Entity::Ref entity)
    {
        auto result = std::make_shared<SelectionContext>();
        result->index = index;
        result->entity = entity;

        return result;
    }

    int index = -1;
    Entity::Ref entity = nullptr;
};

struct IImGuiPanelBase : public Object
{
    BIND_OBJECT(IImGuiPanelBase);

    Scene::Ref scene = nullptr;

    virtual void Draw() = 0;
};

struct EntityPanel: public IImGuiPanelBase
{
    BIND_OBJECT(EntityPanel);

    static Ref Create(Scene::Ref scene)
    {
        auto result = std::make_shared<EntityPanel>();
        result->scene = scene;

        return result;
    }

    char entityIDBuffer[32];
    SelectionContext::Ref selectedEntity = nullptr;

    EntityPanel();

    void Draw() override;

    void DrawEntityList();
    void DrawEntityItem(Entity::Ref entity, int entityIndex);
    void DrawEntityEditor(Entity::Ref entity);

    void DrawTransformComponent(Entity::Ref entity);
    void DrawSpriteComponent(Entity::Ref entity);
};

EntityPanel::EntityPanel()
{
    std::memset(entityIDBuffer, '\0', 32);
}

void EntityPanel::Draw()
{
    bool entityAdded = false;
    if (ImGui::Begin("EntityPanel", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("##entity_name", entityIDBuffer, 32);
        ImGui::SameLine();
        entityAdded = ImGui::Button("Add");

        ImGui::Separator();
        DrawEntityList();

        if (selectedEntity)
        {
            ImGui::Separator();
            DrawEntityEditor(selectedEntity->entity);
        }
    }
    ImGui::End();

    if (entityAdded)
    {
        scene->CreateEntity(entityIDBuffer);
        std::memset(entityIDBuffer, '\0', 32);
    }
}

void EntityPanel::DrawEntityList()
{
    auto entities = scene->registry.view<TagComponent>();

    int entityIndex = 0;
    for (auto entity : entities)
        DrawEntityItem(scene->CreateEntity(entity), entityIndex++);
}

void EntityPanel::DrawEntityItem(Entity::Ref entity, int entityIndex)
{
    auto& tag = entity->GetComponent<TagComponent>();

    bool selected = selectedEntity && selectedEntity->entity == entity;
    if (ImGui::Selectable(tag.id.c_str(), &selected))
    {
        if (selected)
        {
            selectedEntity = SelectionContext::Create(entityIndex, entity);
        }
        else
        {
            selectedEntity = nullptr;
        }
    }
}

void EntityPanel::DrawEntityEditor(Entity::Ref entity)
{
    ImGui::Text("Entity: %s", entity->GetComponent<TagComponent>().id.c_str());
    ImGui::SameLine();

    if (ImGui::Button("+"))
    {
        ImGui::OpenPopup("##add_component");
    }
    ImGui::Separator();

    if (entity->HasComponent<TransformComponent>())
    {
        DrawTransformComponent(entity);
    }

    if (ImGui::BeginPopupContextWindow("##add_component"))
    {
        COMPONENT_MENU_ITEM(TransformComponent);
        COMPONENT_MENU_ITEM(SpriteComponent);
        ImGui::EndPopup();
    }

}

void EntityPanel::DrawTransformComponent(Entity::Ref entity)
{
    ImGui::Text("TransformComponent");
    auto& transform = entity->GetComponent<TransformComponent>();
    ImGui::DragFloat3("Position", &transform.position.x);
}

void EntityPanel::DrawSpriteComponent(Entity::Ref entity)
{
    ImGui::Text("SpriteComponent");
    auto& sprite = entity->GetComponent<SpriteComponent>();
    // TODO(jack): have a set of spritesheets to use
}

struct TileGridPanel: public IImGuiPanelBase
{
    BIND_OBJECT(TileGridPanel);

    static Ref Create(Scene::Ref scene)
    {
        auto result = std::make_shared<TileGridPanel>();
        result->scene = scene;

        return result;
    }

    void Draw() override;
};

void TileGridPanel::Draw()
{

}

int main()
{
    AppWindowSpecification appWindowSpec = {};
    appWindowSpec.title = "Level Editor";
    appWindowSpec.width = 1920;
    appWindowSpec.height = 1080;
    appWindowSpec.api = EGraphicsAPI::OPENGL;

    auto window = AppWindow::Create(appWindowSpec);

    RenderPassSpecification renderPassSpec = {};
    renderPassSpec.clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    renderPassSpec.size = ivec2(1920, 1080);
    renderPassSpec.attachments = {
        { RenderPassSpecification::Attachment::Format::RGBA, RenderPassSpecification::Attachment::Type::COLOR, 1u }
    };

    auto renderPass = window->graphics->CreateRenderPass(renderPassSpec);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    char const* glsl_version = "#version 150";
    //ImGui_ImplWin32_Init(window->handle);
    ImGui_ImplSDL2_InitForOpenGL(window->window, gl::Window::Cast(window)->context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    SceneSpecification sceneSpecification = {};
    sceneSpecification.window = window;
    sceneSpecification.enablePhysics = true;

    auto scene = Scene::Create(sceneSpecification);

    auto entityPanel = EntityPanel::Create(scene);
    auto tileGridPanel = TileGridPanel::Create(scene);

    while (!window->closeRequested)
    {
        renderPass->Begin(nullptr);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        entityPanel->Draw();
        tileGridPanel->Draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        renderPass->End();

        window->PollEvents();
        window->Present();
    }
}