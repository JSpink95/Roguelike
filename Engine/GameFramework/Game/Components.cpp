#include "Game\Components.h"
#include "Game\Entity.h"
#include "Game\Sprite.h"

mat4 TransformComponent::GetMatrix() const
{
    auto transform = mat4(1.0f);

    transform = glm::translate(transform, position);
    transform *= glm::mat4_cast(rotation);
    transform = glm::scale(transform, scale);

    return transform;
}

CameraComponent::CameraComponent(PerspectiveCameraSpecification const& spec)
{
    SetPerspectiveCamera(spec);
}

CameraComponent::CameraComponent(OrthoCameraSpecification const& spec)
{
    SetOrthoCamera(spec);
}

void CameraComponent::SetPerspectiveCamera(PerspectiveCameraSpecification const& spec)
{
    projection = glm::perspectiveFov(glm::radians(spec.fov), spec.size.x, spec.size.y, spec.depth.x, spec.depth.y);
}

void CameraComponent::SetOrthoCamera(OrthoCameraSpecification const& spec)
{
    switch (spec.origin)
    {
        case OrthoCameraSpecification::Origin::Centre:
        {
            float halfSize = spec.size / 2.0f;
            float horizontalSize = halfSize;
            float verticalSize = halfSize * spec.aspectRatio;

            if (spec.flipViewport)
                verticalSize *= -1;

            projection = glm::ortho(-horizontalSize, horizontalSize, -verticalSize, verticalSize, -spec.depth, spec.depth);
        } break;

        case OrthoCameraSpecification::Origin::BottomLeft:
        {
            float horizontalSize = spec.size;
            float verticalSize = spec.size * spec.aspectRatio;

            projection = glm::ortho(
                  0.0f
                , horizontalSize
                , spec.flipViewport ? verticalSize : 0.0f
                , spec.flipViewport ? 0.0f : verticalSize, -spec.depth, spec.depth);
        } break;
    }
}

mat4 CameraComponent::GetProjectionView(Entity::Ref entity) const
{
    auto& transform = owner->GetComponent<TransformComponent>();
    return projection * glm::inverse(transform.GetMatrix());
}
