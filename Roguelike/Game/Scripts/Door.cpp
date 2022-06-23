#include "Game\Scripts\Door.h"

#include <Game\entity.h>
#include <Game\scene.h>

void DoorScript::OnBegin()
{
    // can only get the player here as this is where "owner" should be valid.
    // Also it is more likely the the "player" will have been initialised on the "begin" step of the game loop
    player = GetOwner()->world->FindEntityByTag("player");
}

void DoorScript::OnUpdate(float dt)
{
    // if we're locked, then we can't open the door so we don't need to process the animation
    if (!isLocked)
    {
        UpdateDoorOpenAnimation(dt);
    }
}

float DoorScript::GetDistanceToPlayer() const
{
    // no player? INFINITE DISTANCE
    if (player == nullptr)
        return FLT_MAX;

    auto& owner_transform = owner->GetComponent<TransformComponent>();
    auto& player_transform = player->GetComponent<TransformComponent>();

    return 0.0f;// game::CalculateDistance(owner_transform, player_transform);
}

void DoorScript::UpdateDoorOpenAnimation(float dt)
{
    if (player == nullptr)
        return;

    auto& owner_transform = owner->GetComponent<TransformComponent>();
    auto& player_transform = player->GetComponent<TransformComponent>();

    rotation = glm::mix(rotation, targetRotation, dt * doorOpenSpeed);
    //owner_transform.rotationAboutAnchorPoint = rotation;

    bool was_open = isOpen;
    isOpen = GetDistanceToPlayer() <= 0.8f;

    if (!was_open && isOpen)
    {
        // we are now open, calculate the target rotation
        // this is cheap right now, do better
        //vec2 direction_to_player = glm::normalize(vec2(player_transform.position) - vec2(owner_transform.position));
        //float rot = glm::radians(owner_transform.rotation);
        //float dot = glm::dot(direction_to_player, vec2(glm::sin(rot), -glm::cos(rot)));
        //
        //targetRotation = ((dot < 0.0f) ? 90.0f : -90.0f);
    }

    if (!isOpen)
    {
        targetRotation = 0.0f;
    }
}