#pragma once
#include <Core.h>
#include <Core\Object.h>
#include <entt.hpp>

#include <Game\Sprite.h>

#include <array>
#include <functional>
#include <unordered_map>

struct scene;
struct Entity;
struct Sprite;
struct SpriteGrid;
struct SpriteAsset;
struct IMaterial;
struct IVertexArray;

struct IComponent: public Object
{
    BIND_OBJECT(IComponent);

    std::shared_ptr<Entity> owner;

    virtual ~IComponent() = default;
};

struct TagComponent: public IComponent
{
    BIND_OBJECT(TagComponent);
    std::string id = std::string();

    TagComponent() = default;
    TagComponent(std::string_view tag) : id(tag) {}
};

struct TransformComponent : public IComponent
{
    BIND_OBJECT(TransformComponent);

    TransformComponent() = default;
    TransformComponent(vec3 const& p, vec3 const& s, quat const& q)
        : position(p), scale(s), rotation(q) {}

    mat4 GetMatrix() const;

    vec3 position = vec3(0.0f);
    vec3 scale = vec3(1.0f);
    quat rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);

};

struct PerspectiveCameraSpecification
{
    float fov = 60.0f;
    vec2 size = vec2(0.0f);
    vec2 depth = vec2(0.01f, 100.0f);
};

struct OrthoCameraSpecification
{
    enum class Origin
    {
        Centre, BottomLeft
    };

    bool flipViewport = false;
    float size = 10.0f;
    float depth = 1.0f;
    float aspectRatio = 0.0f;
    Origin origin = Origin::Centre;
};

struct CameraComponent : public IComponent
{
    BIND_OBJECT(CameraComponent);

    CameraComponent() = default;
    CameraComponent(PerspectiveCameraSpecification const& spec);
    CameraComponent(OrthoCameraSpecification const& spec);

    void SetPerspectiveCamera(PerspectiveCameraSpecification const& spec);
    void SetOrthoCamera(OrthoCameraSpecification const& spec);

    mat4 GetProjectionView(std::shared_ptr<Entity> entity) const;

    mat4 projection = mat4(1.0f);
};

struct SpriteComponent: public IComponent
{
    BIND_OBJECT(SpriteComponent);

    SpriteComponent() = default;

    Sprite::Ref sprite = nullptr;
};
