#include "Scripts\PlayerController.h"
#include <Game\Entity.h>
#include <Game\physics_components.h>

#include <box2d\box2d.h>

constexpr float MAX_SPEED = 10.0f;
constexpr float ACCELERATION = 200.0f;
constexpr float JUMPFORCE = 200.0f;

PlayerController::PlayerController(KeyboardInputManager::Ref keyboard)
    : inputHandler(PlayerInputHandler::Create(keyboard))
{
}

void PlayerController::OnBegin()
{
    inputHandler->BindLeftMovementInput(SDLK_LEFT);
    inputHandler->BindRightMovementInput(SDLK_RIGHT);
    inputHandler->BindJumpMovementInput(SDLK_SPACE);
}

void PlayerController::OnUpdate(float dt)
{
    auto& rigidbody = owner->GetComponent<RigidBodyComponent>();

    vec2 movement = { 0.0f, 0.0f };
    if (inputHandler->moveLeftCommand->inputDown)
        movement.x -= 1.0f;

    if (inputHandler->moveRightCommand->inputDown)
        movement.x += 1.0f;

    if (inputHandler->jumpCommand->inputDown)
        movement.y += 1.0f;

    b2Body* body = static_cast<b2Body*>(rigidbody.runtimeBody);
    body->ApplyForceToCenter(b2Vec2(movement.x * ACCELERATION, movement.y * JUMPFORCE), true);

    // limit our horizontal speed
    if (glm::abs(body->GetLinearVelocity().x) >= MAX_SPEED)
    {
        float clampedLVX = glm::sign(body->GetLinearVelocity().x) * MAX_SPEED;
        body->SetLinearVelocity(b2Vec2(clampedLVX, body->GetLinearVelocity().y));
    }
}